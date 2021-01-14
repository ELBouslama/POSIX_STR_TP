#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pthread.h"

#define LOGGING 0
#define TEST_STRUCTURES 1

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

typedef struct
{
    char *name;
    pthread_mutex_t *stationLock;
} station;

typedef struct
{
    char *name;
    station *stations;
    int capacity;
    int size;
} ligne;

typedef struct
{
    char *name;
    ligne *ligne;

} metro;

//global variables initializations.
ligne lignes[5];
station *stations[21];
metro metros[4];

station *createStation(char *name)
{
    station *s = (station *)malloc(sizeof(station));
    s->name = (char *)malloc(sizeof(char) * 25);
    strcpy(s->name, name);
    s->stationLock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(s->stationLock, NULL);

    return s;
}

ligne *createLigne(char *name, int size)
{
    ligne *l = (ligne *)malloc(sizeof(ligne));

    l->size = size;
    l->stations = (station *)malloc(sizeof(station) * size);
    l->name = (char *)malloc(sizeof(char) * 25);
    l->capacity = 0;
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

void addStation(ligne *l, station *s)
{
    if (l->capacity < l->size)
        l->stations[l->capacity++] = *s;
}

void ShowLigneDetails(ligne *l)
{
    printf("%s ==> Capacity %d ----- Size %d ==> Stations :\n", l->name, l->capacity, l->size);
    for (int index = 0; index < l->capacity; index++)
    {
        station *s = (l->stations + index);
        printf("\t\v  %s \n", s->name);
    }
}

void initScene()
{
    char stationIndex = 'A';
    char name[8];
    strcpy(name, "Station");
    for (int a = 0; a < 21; a++)
    {
        name[7] = stationIndex++;
        stations[a] = createStation(name);
    }

#if LOGGING
    for (int index = 0; index < 21; index++)
    {
        station *s = stations[index];
        printf("\t %d \v  %s \n", index, s->name);
    }
#endif

    stationIndex = 'A';
    strcpy(name, "Ligne");
    name[6] = '\0';
    for (int index = 0; index < 5; index++)
    {
        name[5] = stationIndex++;
        lignes[index] = *createLigne(name, 10);
    }

    //set stations for Ligne Blue
    addStation(&lignes[0], stations[1]);
    addStation(&lignes[0], stations[2]);
    addStation(&lignes[0], stations[8]);
    addStation(&lignes[0], stations[17]);
    addStation(&lignes[0], stations[18]);
    addStation(&lignes[0], stations[19]);

    //set stations for Ligne Red
    addStation(&lignes[1], stations[4]);
    addStation(&lignes[1], stations[5]);
    addStation(&lignes[1], stations[6]);
    addStation(&lignes[1], stations[7]);
    addStation(&lignes[1], stations[8]);
    addStation(&lignes[1], stations[9]);

    //set stations for Ligne Marron
    addStation(&lignes[2], stations[3]);
    addStation(&lignes[2], stations[10]);

    //set stations for Ligne Green
    addStation(&lignes[3], stations[0]);
    addStation(&lignes[3], stations[15]);
    addStation(&lignes[3], stations[11]);
    addStation(&lignes[3], stations[6]);
    addStation(&lignes[3], stations[12]);
    addStation(&lignes[3], stations[13]);
    addStation(&lignes[3], stations[14]);

    //set stations for Ligne Black
    addStation(&lignes[4], stations[16]);
    addStation(&lignes[4], stations[20]);

    for (int index = 0; index < 5; index++)
    {
        ShowLigneDetails(&lignes[index]);
    }
}

void *runMetro(void *mt)
{
    metro *m = (metro *)mt;
    printf("Starting Metro : %s  \n", m->name);
    station *stations = m->ligne->stations;
    for (int index = 0; index < m->ligne->capacity; index++)
    {
        station *s = (m->ligne->stations + index);
        pthread_mutex_lock(s->stationLock);
        printf("METRO" ANSI_COLOR_RED " %s " ANSI_COLOR_RESET "IS ON ..." ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET "\n", m->name, s->name);
        usleep(500000);
        pthread_mutex_unlock(s->stationLock);
    }
}

int main(int argc, char *argv)
{
#if TEST_STRUCTURES

    station *s = createStation("Station1");
    printf("%s \n", s->name);

    ligne *l = createLigne("Ligne1", 10);
    addStation(l, s);
    ShowLigneDetails(l);

#endif

    initScene();

    metro *m1 = createMetro("Metro1", &lignes[0]);
    printf("%s ===> Ligne : %s \n", m1->name, m1->ligne->name);

    metro *m2 = createMetro("Metro2", &lignes[1]);
    printf("%s ===> Ligne : %s \n", m2->name, m2->ligne->name);

    metro *m3 = createMetro("Metro2", &lignes[2]);
    printf("%s ===> Ligne : %s \n", m3->name, m3->ligne->name);

    metro *m4 = createMetro("Metro3", &lignes[3]);
    printf("%s ===> Ligne : %s \n", m4->name, m4->ligne->name);

    metro *m5 = createMetro("Metro4", &lignes[4]);
    printf("%s ===> Ligne : %s \n", m5->name, m5->ligne->name);

    pthread_t m_1, m_2, m_3, m_4, m_5;
    pthread_create(&m_1, NULL, runMetro, m1);
    pthread_create(&m_2, NULL, runMetro, m2);
    pthread_create(&m_5, NULL, runMetro, m5);
    pthread_create(&m_4, NULL, runMetro, m4);
    pthread_create(&m_3, NULL, runMetro, m3);

    pthread_join(m_1, NULL);
    pthread_join(m_2, NULL);
    pthread_join(m_3, NULL);
    pthread_join(m_4, NULL);
    pthread_join(m_5, NULL);
    return 0;
}