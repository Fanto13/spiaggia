#include "prototipazioni.h"
void visualizza_date(list_data *f)
{
	list_data *tmp = f;
	while(tmp!=NULL)
	{
		printf("%d/%d/%d\n",tmp->data_inizio.giorno,tmp->data_inizio.mese,tmp->data_inizio.anno );
		printf("%d/%d/%d\n",tmp->data_fine.giorno,tmp->data_fine.mese,tmp->data_fine.anno );
		tmp = tmp->next;
	}
}
bool controllo_data(data day) /*ritorna true se il numero del giorno ?accettato, altrimenti false*/
{
	giorni_mese g;

	if(day.anno<2017 || day.anno>2019) return false;

	switch(day.mese)
	{
		case 1: 
		{
			g=gennaio;
			if(day.giorno<=g)
			{
				return true;
			}
			break;
		}
		case 2: 
		{
			g = febbraio;
			if(day.giorno<=g)
			{
				return true;
			}
			break;
		}
		case 3: 
		{
			g=marzo;
			if(day.giorno<=g)
			{
				return true;
			}
			break;
		}
		case 4: 
		{
			g = aprile;
			if(day.giorno<=g)
			{
				return true;
			}
			break;
		}
		case 5: 
		{
			g=maggio;
			if(day.giorno<=g)
			{
				return true;
			}
			break;
		}
		case 6: 
		{
			g = giugno;
			if(day.giorno<=g)
			{
				return true;
			}
			break;
		}
		case 7: 
		{
			g=luglio;
			if(day.giorno<=g)
			{
				return true;
			}
			break;
		}
		case 8: 
		{
			g = agosto;
			if(day.giorno<=g)
			{
				return true;
			}
			break;
		}
		case 9: 
		{
			g=settembre;
			if(day.giorno<=g)
			{
				return true;
			}
			break;
		}
		case 10: 
		{
			g = ottobre;
			if(day.giorno<=g)
			{
				return true;
			}
			break;
		}
		case 11: 
		{
			g=novembre;
			if(day.giorno<=g)
			{
				return true;
			}
			break;
		}
		case 12: 
		{
			g = dicembre;
			if(day.giorno<=g)
			{
				return true;
			}
			break;
		}

	}
	return false;
}
int confronto_date(data start, data end) /*ritorna 0-> intervallo valido    ritorna 1-> intervallo non valido		ritorna 2-> date uguali*/
{										 
	if(start.anno < end.anno)			 
	{
		return 0;
	}
	else if(start.anno == end.anno)
	{
		if(start.mese < end.mese)
		{
			return 0;
		}
		else if(start.mese == end.mese)
		{
			if(start.giorno < end.giorno)
			{
				return 0;
			}
			else if(start.giorno == end.giorno)
			{
				return 2;
			}
		}
	}
	return 1;
}
bool controllo_intervallo(data first_start, data first_end, data second_start, data second_end)
{
	if(confronto_date(first_end,second_start)==0)
	{
		return true;
	}
	else if(confronto_date(second_end,first_start)==0)
	{
		return true;
	}

	return false;
}

data string_to_date(char* string)
{
	int j,tmp;
	char str[5] = "";
	data ret;
	for(j = 0; j < 2; j++)
	{
		str[j] = string[j];
	}
	//str[2] = '\0';
	
	tmp = atoi(str);
	ret.giorno = tmp;

	for(j = 0; j < 2; j++)
	{
		str[j] = string[3+j];
	}
	//str[2] = '\0';
	tmp = atoi(str);
	ret.mese = tmp;

	for(j = 0; j < 4; j++)
	{
		str[j] = string[j+6];
	}

	tmp = atoi(str);
	ret.anno = tmp;

	return ret;

}
