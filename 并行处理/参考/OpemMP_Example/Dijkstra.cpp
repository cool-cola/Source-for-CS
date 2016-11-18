#include <omp.h>
#include <iostream>

#define NUM_VERTEX 4096*4

bool *g_pVertexVisitTag_S;
bool *g_pVertexVisitTag_M;
int  g_NumThreads;
int  g_NumVertexPerChunk; 
int  g_CurrentMinDis;
int  g_CurrentMinVertex;

unsigned int **g_ppDirectDis;
unsigned int *g_pMinDistance_S;
unsigned int *g_pMinDistance_M;

//*********************************************************************************
//FUNCTION:
void initUndirectedGraph()
{
	srand(0);

	g_pVertexVisitTag_M = new bool[NUM_VERTEX];
	g_pVertexVisitTag_S = new bool[NUM_VERTEX];
	g_pMinDistance_M = new unsigned int[NUM_VERTEX];
	g_pMinDistance_S = new unsigned int [NUM_VERTEX];
	g_ppDirectDis = new unsigned int*[NUM_VERTEX];
	for (unsigned int i=0; i<NUM_VERTEX; i++) g_ppDirectDis[i] = new unsigned int[NUM_VERTEX];

	for (unsigned int i=0; i<NUM_VERTEX; i++)
	{
		for(unsigned int k=i; k<NUM_VERTEX; k++)
		{
			if (k == i)
			{
				g_ppDirectDis[i][k] = 0; 
			}
			else
			{
				if ((rand() % 2 == 0))
				{
					g_ppDirectDis[i][k] = (rand() % 20 + 1);
				}
				else
				{
					g_ppDirectDis[i][k] = INT_MAX;
				}
				g_ppDirectDis[k][i] = g_ppDirectDis[i][k];
			}
		}
		g_pVertexVisitTag_S[i] = false;
		g_pVertexVisitTag_M[i] = false;
		g_pMinDistance_S[i] = g_pMinDistance_M[i] = g_ppDirectDis[0][i];
	}

	g_pVertexVisitTag_S[0] = g_pVertexVisitTag_M[0] = true;
	g_pMinDistance_S[0] = g_pMinDistance_M[0] = 0;
}

//*********************************************************************************
//FUNCTION:
void clearResult()
{
	for (unsigned int i=1; i<NUM_VERTEX; i++)
	{
		g_pVertexVisitTag_M[i] = false;
		g_pMinDistance_M[i] = g_ppDirectDis[0][i];
	}
}

//*********************************************************************************
//FUNCTION:
std::pair<unsigned int, unsigned int> findMinVertexDistance(int vStartVertex, int vEndVertex, bool *vVisitTag, unsigned int* vCurrentMinDis) 
{
	unsigned int MinDis = INT_MAX;
	unsigned int VertexIndex = 0;
	for (int i=vStartVertex; i<=vEndVertex; i++)
	{
		if (!vVisitTag[i] && vCurrentMinDis[i] < MinDis)
		{
			MinDis = vCurrentMinDis[i];
			VertexIndex = i;
		}
	}
	return std::make_pair(MinDis, VertexIndex);
}

//*********************************************************************************
//FUNCTION:
void updateMinDistance(int vStartVertex, int vEndVertex, unsigned int* vCurrentMinDis) 
{
	for (int i=vStartVertex; i<=vEndVertex; i++)
	{
		if (vCurrentMinDis[g_CurrentMinVertex] + g_ppDirectDis[i][g_CurrentMinVertex] < vCurrentMinDis[i])
		{
			vCurrentMinDis[i] = vCurrentMinDis[g_CurrentMinVertex] +  g_ppDirectDis[i][g_CurrentMinVertex];
		}
	}
}

//*********************************************************************************
//FUNCTION:
void findShortestPathDijkstra_OpenMP()
{
	omp_set_num_threads(g_NumThreads);
#pragma omp parallel
	{
	#pragma omp single
		{
			g_NumVertexPerChunk = NUM_VERTEX / g_NumThreads;
		}

		int ThreadID = omp_get_thread_num();

		int StartVertices = ThreadID * g_NumVertexPerChunk;
		int EndVertices  = StartVertices + g_NumVertexPerChunk - 1;

		for (int Step=0; Step<NUM_VERTEX; Step++)
		{
		#pragma omp single
			{
				g_CurrentMinDis = INT_MAX;
				g_CurrentMinVertex = 0;
			}
			
			std::pair<unsigned int, unsigned int> t = findMinVertexDistance(StartVertices, EndVertices, g_pVertexVisitTag_M, g_pMinDistance_M);
			unsigned int CurMinDistance = t.first;
			unsigned int CurMinVertex = t.second;

		#pragma omp critical
			{
				if (CurMinDistance < g_CurrentMinDis)
				{
					g_CurrentMinDis = CurMinDistance;
					g_CurrentMinVertex = CurMinVertex;
				}
			}
		#pragma omp barrier
		#pragma omp single
			{
				g_pVertexVisitTag_M[g_CurrentMinVertex] = true;
			}

			updateMinDistance(StartVertices, EndVertices, g_pMinDistance_M);

		#pragma omp barrier
		}
	}
}

//*********************************************************************************
//FUNCTION:
void findShortestPathDijkstra_Serial()
{
	unsigned int StartVertices = 0;
	unsigned int EndVertices  = NUM_VERTEX-1;

	for (int Step=0; Step<NUM_VERTEX; Step++)
	{
		g_CurrentMinDis = INT_MAX;
		g_CurrentMinVertex = 0;

		std::pair<unsigned int, unsigned int> t = findMinVertexDistance(StartVertices, EndVertices, g_pVertexVisitTag_S, g_pMinDistance_S);
		unsigned int CurMinDistance = t.first;
		unsigned int CurMinVertex = t.second;

		if (CurMinDistance < g_CurrentMinDis)
		{
			g_CurrentMinDis = CurMinDistance;
			g_CurrentMinVertex = CurMinVertex;
		}

		g_pVertexVisitTag_S[g_CurrentMinVertex] = true;

		updateMinDistance(StartVertices, EndVertices, g_pMinDistance_S);
	}
}

//*********************************************************************************
//FUNCTION:
void checkResult()
{
	for (int i=1; i<NUM_VERTEX; i++)
	{
 		if (g_pMinDistance_S[i] != g_pMinDistance_M[i])
		{
			std::cout << "Incorrect results!!!" << std::endl;
			break;
		}
	}
}

//*********************************************************************************
//FUNCTION:
int main(int vArgC, char** vArgV)
{
	initUndirectedGraph();

	std::cout << "Number of vertices:  " << NUM_VERTEX << std::endl;

	double StartTime = omp_get_wtime();
	findShortestPathDijkstra_Serial();
	double EndTime = omp_get_wtime();
	std::cout << "Elapsed time for serial version:  " << EndTime - StartTime << std::endl;

	g_NumThreads = 2;
	StartTime = omp_get_wtime();
	findShortestPathDijkstra_OpenMP();
	EndTime = omp_get_wtime();
	std::cout << "Elapsed time for OpenMP version with " << g_NumThreads << " threads :  " << EndTime - StartTime << std::endl;
	checkResult();
	clearResult();

	g_NumThreads = 4;
	StartTime = omp_get_wtime();
	findShortestPathDijkstra_OpenMP();
	EndTime = omp_get_wtime();
	std::cout << "Elapsed time for OpenMP version with " << g_NumThreads << " threads :  " << EndTime - StartTime << std::endl;
	checkResult();
	clearResult();

	g_NumThreads = 8;
	StartTime = omp_get_wtime();
	findShortestPathDijkstra_OpenMP();
	EndTime = omp_get_wtime();
	std::cout << "Elapsed time for OpenMP version with " << g_NumThreads << " threads :  " << EndTime - StartTime << std::endl;
	checkResult();

	delete[] g_pMinDistance_M;
	delete[] g_pMinDistance_S;
	delete[] g_pVertexVisitTag_M;
	delete[] g_pVertexVisitTag_S;
	for (unsigned int i=0; i<NUM_VERTEX; i++) delete[] g_ppDirectDis[i];
	delete[] g_ppDirectDis;

	return 0;
}