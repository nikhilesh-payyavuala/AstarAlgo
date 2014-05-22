#include <stdio.h>
#include <stdlib.h>
//#include "heap.c"

typedef struct { int * x;int *y; int pri; } q_elem_t;

typedef struct { q_elem_t *buf; int n, alloc; } pri_queue_t, *pri_queue;
#define priq_purge(q) (q)->n = 1
#define priq_size(q) ((q)->n - 1)

pri_queue priq_new(int size)


{
  if (size < 4) size = 4;

 
  pri_queue q = malloc(sizeof(pri_queue_t));


  q->buf = malloc(sizeof(q_elem_t) * size);


  q->alloc = size;
  q->n = 1;
 
  return q;

}
 
void priq_push(pri_queue q, int *x, int* y, int pri)

{
  q_elem_t *b;
  int n, m;
 
  if (q->n >= q->alloc) {


    q->alloc *= 2;
    b = q->buf = realloc(q->buf, sizeof(q_elem_t) * q->alloc);


  } else
    b = q->buf;
 
  n = q->n++;

  /* append at end, then up heap */
  while ((m = n / 2) && pri < b[m].pri) {


    b[n] = b[m];
    n = m;
  }

  b[n].x = x;
  b[n].y=y;
  b[n].pri = pri;

}
 
/* remove top item. returns 0 if empty. *pri can be null. */
q_elem_t  priq_pop(pri_queue q, int *pri)


{
  q_elem_t out;
  if (q->n == 1) 
 {
  out.x=0;
  out.y=0;
  out.pri=0;
  return out;
 }


 
  q_elem_t *b = q->buf;
 
  out = b[1];

  if (pri) *pri = b[1].pri;

 
  /* pull last item to top, then down heap. */
  --q->n;
 
  int n = 1, m;

  while ((m = n * 2) < q->n) {

    if (m + 1 < q->n && b[m].pri > b[m + 1].pri) m++;


 
    if (b[q->n].pri <= b[m].pri) break;


    b[n] = b[m];
    n = m;
  }

 
  b[n] = b[q->n];
  if (q->n < q->alloc / 2 && q->n >= 16)


    q->buf = realloc(q->buf, (q->alloc /= 2) * sizeof(b[0]));


 
  return out;
}



#define MAPW 16
#define MAPH 12

struct list_s
{
    int x;
    int y;
    int f;
} path[10000];

struct NODE
{
    int onopen;
	int onclosed;
	int walkable;
    int x; //nodes coordiates
    int y;
    int g;
    int h;
    int f;
    int parentx;
    int parenty;
} node[MAPW][MAPH];

int count;

void initnodes()
{
    int x,y;
    for (x=0;x<MAPW;x++)
    {
        for (y=0;y<MAPH;y++)
        {
            node[x][y].onopen=0;
            node[x][y].onclosed=0;
            if((x+y)%2==0)
	    node[x][y].walkable = 0;
    else
      node[x][y].walkable = 1;

            node[x][y].x = x;
            node[x][y].y = y;
            node[x][y].g = 0;
            node[x][y].h = 0;
            node[x][y].f = 0;
            node[x][y].parentx = 0;
            node[x][y].parenty = 0;
        }
    }
}



struct list_s *findpath(int startx, int starty, int endx, int endy)
{
    int x=0,y=0; // for running through the nodes
    int dx,dy; // for the 8 squares adjacent to each node
    int p;
    int cx=startx, cy=starty;
    int lowestf=10000;
    int entered_while=0;
    q_elem_t c;
    int tentg;

     pri_queue q = priq_new(0);
     node[startx][starty].g=0;
     node[startx][starty].h=(abs(endx-(cx))+abs(endy-(cy)))*10;
     node[startx][starty].f=node[startx][starty].g+ node[startx][starty].h;
     priq_push(q, &node[startx][starty].x, &node[startx][starty].y, node[startx][starty].f);


      c = priq_pop(q, &p);
      printf("top of list is %d,%d with f=%d\n",*(c.x),*(c.y),c.pri);
  /* pop them and print one by one */
      while (entered_while<100)
		{
                        printf("cx=%d and cy=%d\n",cx,cy);
                        //c = priq_pop(q, &p);
                        entered_while+=1;
                        //printf("entering for %d time\n",entered_while);
			//cx=*(c.x);
			//cy=*(c.y);
                        //printf("check1\n");
			if(cx==endx && cy==endy){
				printf("hurray\n");
				break;
			}
			else{
                              //printf("check1\n");
                              node[cx][cy].onopen=0;
                              node[cx][cy].onclosed=1;
				//remove curent node from open and put it in closed
        int tid ;
        #pragma omp parallel shared(node,q) private( dx,tid,tentg,dy)
  {
        #pragma omp for                        
	for (dx=-1;dx<=1;dx++)
        			{
            for (dy=-1;dy<=1;dy++)
            {
              tid= omp_get_thread_num();
               printf("cx=%d: cy=%d: dx = %d:dy = %d:thread=%d\n",cx,cy, dx,dy,tid);
                if ((dx!=0) || (dy!=0))
                {
                    if ((cx+dx)<MAPW && (cx+dx>-1) && (cy+dy)<MAPH && (cy+dy)>-1)
                    {
                        // if its walkable and not on the closed list
                        //printf("home node is %d,%d thread %d entered %d,%d check2\n",cx,cy,tid,dx,dy);
                        if (node[cx+dx][cy+dy].walkable==0 && node[cx+dx][cy+dy].onclosed==0)
                        {
                            if (dx!=0 && dy!=0) tentg = node[cx][cy].g + 14; // diagonals cost 14
                                else tentg = node[cx][cy].g + 10;

                             //printf("check3\n");
                            //if its not on open list
                            if (node[cx+dx][cy+dy].onopen==0 || tentg < node[cx+dx][cy+dy].g )
                            {
                                //printf("home node is %d,%d thread %d entered %d,%d check3\n",cx,cy,tid,cx+dx,cy+dy);
                                node[cx+dx][cy+dy].parentx = node[cx][cy].x ;
                                node[cx+dx][cy+dy].parenty = node[cx][cy].y ;
                                node[cx+dx][cy+dy].g = tentg;
                                node[cx+dx][cy+dy].h =
                                        (abs(endx-(cx+dx))+abs(endy-(cy+dy)))*10;
                                node[cx+dx][cy+dy].f = node[cx+dx][cy+dy].g +  node[cx+dx][cy+dy].h;
                                if(node[cx+dx][cy+dy].onopen==0 ) {

                                     priq_push(q, &node[cx+dx][cy+dy].x, &node[cx+dx][cy+dy].y, node[cx+dx][cy+dy].f);
                                     //printf("home node is %d,%d thread %d entered %d,%d check4\n",cx,cy,tid,cx+dx,cy+dy);
                                	node[cx+dx][cy+dy].onopen==1;
                                	//push it in
                                }
                               
                                          
                            }
                            //otherwise it is on the open list
                          
                        }// end if walkable and not on closed list
                    }
                }
            }
        }//end for each 8 adjacent node
            #pragma omp barrier
      }//end of pragma
			}
                c = priq_pop(q, &p);
                 cx=*(c.x);
			cy=*(c.y);
		   
		}
     


    printf("entered while %d times\n",entered_while); 
    count=0;
    cx = endx;
    cy = endy;
    printf("endx is %d and endy is %d\n",endx,endy);
    while (cx!=startx || cy!=starty)
    {
        path[count].x = node[cx][cy].parentx;
        path[count].y = node[cx][cy].parenty;
        path[count].f = node[cx][cy].f;
        cx = path[count].x;
        cy = path[count].y;
        printf("final path is %d:%d\n",path[count].x ,path[count].y );
        count++;
        if (count>100) break;
    }
    printf("length of path is %d\n",count);
    return path; //we're done, return a pointer to the final path;
}

int main(int argc, char* argv[])
{
	 struct list_s *thepath;
	 initnodes();
	 thepath = findpath(0,0,4,4);
	 return 0;
}

