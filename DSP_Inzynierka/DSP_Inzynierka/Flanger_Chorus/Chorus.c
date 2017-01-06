/*
 * flanger.c
 *
 *  Created on: 9 pa� 2016
 *      Author: �ukasz Pindor
 */
#include "math.h"
#include "dsplib.h"
#include "usbstk5515.h"
#include <stdlib.h>
#define N 1205


int delay_chorus_w_probkach=0;
char licznik_ch=0;
int tablica_opoznienia_chorus[N];
signed long temp2_ch=0;
signed long temp3_ch=0;
signed long temp4_ch=0;

int omega_ch=0;
short int faza_ch = 0;
DATA y_ch = 0;


void chorus_init(char* opoznienie,  char* lfo_frequency) {

	omega_ch=(int)((22368L * ((int)(*lfo_frequency))) >> 14);
	short int* i_ch=(short int*)malloc(11);
	// czyszczenie buffora, w kt�rym b�d� przechowywane pr�bki, kt�re s� dodawane w odpowiednim czasie
	for ( *i_ch = 0 ; *i_ch < N ; *i_ch++)
	{
		tablica_opoznienia_chorus[*i_ch] = 0;
	}
	// zmienna odpowiedzialna za op�nienie
	delay_chorus_w_probkach=*opoznienie*48;
	// 1ms = 48 pr�bek
}

Int16 chorus (Int16* sygnal_wejsciowy, char* lfo_amplitude, char* volume)
{


	sine(&faza_ch, &y_ch, 1);
	faza_ch += omega_ch;

	temp2_ch=((long)(*lfo_amplitude)*327*y_ch);
	temp2_ch>>=15;
	y_ch=((int)(temp2_ch));

	if (y_ch<-32760)
		y_ch=32760;
	else if(y_ch>=-32760)
		y_ch=abs(y_ch);


	temp3_ch=((long)y_ch*delay_chorus_w_probkach);
	temp3_ch>>=15;
	temp3_ch=licznik_ch+temp3_ch;


	if(temp3_ch<0)
		temp3_ch=temp3_ch+delay_chorus_w_probkach;
	else if (temp3_ch>=delay_chorus_w_probkach)
		temp3_ch=temp3_ch-delay_chorus_w_probkach;


	temp2_ch=*sygnal_wejsciowy+tablica_opoznienia_chorus[temp3_ch];
	temp2_ch=((long)(*volume)*327*(temp2_ch));
	temp2_ch>>=15;
		if(delay_chorus_w_probkach>0)
		{
			if((temp3_ch+144)<delay_chorus_w_probkach)
				temp4_ch=*sygnal_wejsciowy+tablica_opoznienia_chorus[temp3_ch+144];
			else if((temp3_ch+144)>delay_chorus_w_probkach)
				temp4_ch=*sygnal_wejsciowy+tablica_opoznienia_chorus[temp3_ch+144-delay_chorus_w_probkach];

			if((*volume)<100 && (*volume)>0)
				{
				temp4_ch=((long)((*volume)*327*(temp4_ch)+3276));
				temp4_ch>>=15;
				}

			else if((*volume)==100 && (*volume)==0)
				{
				temp4_ch=((long)(*volume)*327*(temp3_ch));
				temp4_ch>>=15;
				}
		}
	tablica_opoznienia_chorus[licznik_ch]=*sygnal_wejsciowy;
	licznik_ch++;
	if(licznik_ch==delay_chorus_w_probkach)
		licznik_ch=0;
	temp2_ch=temp2_ch+temp4_ch;

	return (Int16) temp2_ch;
}


