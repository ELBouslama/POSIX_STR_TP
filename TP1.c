#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#define TRAINS_NUMBER_MR1 2
#define TRAINS_NUMBER_MR2 1
#define TRAINS_NUMBER_MR3 5

typedef struct
{
    char *name;
    pthread_mutex_t *lock;
} segment;

typedef struct
{
    char *name;
    segment *segments;
    int segments_count;
    int size;
} ligne;
typedef struct
{
    char *name;
    ligne *ligne;

} metro;

segment *createSegment(char *name)
{
    segment *s = (segment *)malloc(sizeof(segment));
    s->name = (char *)malloc(sizeof(char) * 25);
    strcpy(s->name, name);
    s->lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(s->lock, NULL);

    return s;
}

ligne *createLigne(char *name, int size)
{
    ligne *l = (ligne *)malloc(sizeof(ligne));

    l->size = size;
    l->segments = (segment *)malloc(sizeof(segment) * size);
    l->name = (char *)malloc(sizeof(char) * 25);
    l->segments_count = 0;
    strcpy(l->name, name);
    return l;
}
metro *createMetro(char *name, ligne *l)
{
    metro *m = (metro *)malloc(sizeof(metro));
    m->name = (char *)malloc(sizeof(char) * 25);
    strcpy(m->name, name);
    m->ligne = l;
    return m;
}

void addSegment(ligne *l, segment *s)
{

    l->segments[l->segments_count++] = *s;
}

void *runMetro(void *mt)
{
    metro *m = (metro *)mt;
    printf("debut Metro : %s  \n", m->name);
    segment *segments = m->ligne->segments;
    for (int index = 0; index < m->ligne->segments_count; index++)
    {
        segment *s = (m->ligne->segments + index);
        pthread_mutex_lock(s->lock);
        printf("Metro %s entre dans le segment %s \n", m->name, s->name);
        // 6 seconds in segment
        usleep(6000000);
        printf("Metro %s quitte le segment %s \n", m->name, s->name);
        pthread_mutex_unlock(s->lock);
    }
}

void create_threads_for_line(ligne *l, int line_number, pthread_t *threads, int number_of_trains)
{
    for (int metro_number = 0; metro_number < number_of_trains; metro_number++)
    {
        char metro_name[30];
        // M{Metro number on this line}L{line number}
        snprintf(metro_name, 30, "M%dL%d", metro_number, line_number);

        metro *m1 = createMetro(metro_name, l);
        pthread_create(&threads[metro_number], NULL, runMetro, m1);
    }
}
void join_threads_for_line(pthread_t *threads, int number_of_trains)
{
    for (int index = 0; index < number_of_trains; index++)
    {
        pthread_join(threads[index], NULL);
    }
}
int main()
{
    ligne *MR1 = createLigne("MR1", 3);
    ligne *MR2 = createLigne("MR2", 2);
    ligne *MR3 = createLigne("MR3", 2);

    segment *AB = createSegment("AB");
    segment *BC = createSegment("BC");
    segment *CD = createSegment("CD");
    segment *EC = createSegment("EC");
    segment *BF = createSegment("BF");

    addSegment(MR1, AB);
    addSegment(MR1, BC);
    addSegment(MR1, CD);
    addSegment(MR2, BF);
    addSegment(MR2, AB);
    addSegment(MR3, EC);
    addSegment(MR3, BC);

    pthread_t MR1_threads[TRAINS_NUMBER_MR1];
    pthread_t MR2_threads[TRAINS_NUMBER_MR2];
    pthread_t MR3_threads[TRAINS_NUMBER_MR3];
    create_threads_for_line(MR1, 1, MR1_threads, TRAINS_NUMBER_MR1);
    create_threads_for_line(MR2, 2, MR2_threads, TRAINS_NUMBER_MR2);
    create_threads_for_line(MR3, 3, MR3_threads, TRAINS_NUMBER_MR3);
    join_threads_for_line(MR1_threads, TRAINS_NUMBER_MR1);
    join_threads_for_line(MR2_threads, TRAINS_NUMBER_MR2);
    join_threads_for_line(MR3_threads, TRAINS_NUMBER_MR3);

    return 0;
}