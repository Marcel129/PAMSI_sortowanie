#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <vector>
#include <fstream>


 //opcje wype³nienia tabeli
#define RAND 0
#define SORTED_25 1
#define SORTED_50 2
#define SORTED_75 3
#define SORTED_95 4
#define SORTED_99 5
#define SORTED_997 6
#define INVERTED_SORT 7

using namespace std;


__int64 currentTimeMillis()
{
	static const __int64 magic = 116444736000000000; // 1970/1/1
	SYSTEMTIME st;
	GetSystemTime(&st);
	FILETIME   ft;
	SystemTimeToFileTime(&st, &ft); // in 100-nanosecs...
	__int64 t;
	memcpy(&t, &ft, sizeof t);
	return (t - magic) / 10; // scale to millis., if 10 - micros
}

/**************************************************************************************************************************
*
*                                                          QUICKSORT
*
***************************************************************************************************************************/

unsigned long int split_array(vector<long int>& tab, unsigned long int first_elem_index, unsigned long int last_elem_index) {

	unsigned long int pivot_index = first_elem_index + (last_elem_index - first_elem_index) / 2;//pivot jako ostatni element tablicy
	long int pivot = tab[pivot_index];
	unsigned long int actuall_index = first_elem_index;//zaczynamy od pierwszego elementu

	swap(tab[pivot_index], tab[last_elem_index]);//wyrzuæ pivot na koniec
	for (unsigned long int i = first_elem_index; i < last_elem_index; i++) {
		if (tab[i] < pivot) {
			swap(tab[i], tab[actuall_index]);
			actuall_index++;
		}
	}
	swap(tab[actuall_index], tab[last_elem_index]);//wyrzuæ pivot na swoje miejsce

	return actuall_index;
}

void quicksort(vector<long int>& tab, unsigned long int first_elem_index, unsigned long int last_elem_index) {

	unsigned long int pivot_index;

	if (first_elem_index < last_elem_index) {
		pivot_index = split_array(tab, first_elem_index, last_elem_index);
		if (pivot_index != first_elem_index) quicksort(tab, first_elem_index, pivot_index - 1);
		if (pivot_index != last_elem_index) quicksort(tab, pivot_index + 1, last_elem_index);
	}
}

/**************************************************************************************************************************
*
*                                                          MERGESORT
*
***************************************************************************************************************************/

void merge(vector<long int>& tab, unsigned long int first_elem_index, unsigned long int mid_elem_index, unsigned long int last_elem_index) {

	unsigned long int left_length = mid_elem_index - first_elem_index + 1, right_length = last_elem_index - mid_elem_index;//d³ugoœci lewej i prawej czêœci
	vector<long int> left_array, right_array;//tablice pomocnicze

	for (unsigned long int i = 0; i < left_length; i++) left_array.push_back(tab[first_elem_index + i]);
	for (unsigned long int i = 0; i < right_length; i++) right_array.push_back(tab[mid_elem_index + i + 1]);

	unsigned long int left = 0, right = 0, actuall_position = first_elem_index;//zmiennne do przechodzenia po indeksach elementów

	while (left < left_length && right < right_length)
	{
		if (left_array[left] <= right_array[right]) {
			tab[actuall_position] = left_array[left];
			left++;
		}
		else {
			tab[actuall_position] = right_array[right];
			right++;
		}
		actuall_position++;
	}

	while (left < left_length) {
		tab[actuall_position] = left_array[left];
		left++;
		actuall_position++;
	}

	while (right < right_length) {
		tab[actuall_position] = right_array[right];
		right++;
		actuall_position++;
	}

}

void mergesort(vector<long int>& tab, unsigned long int first_elem_index, unsigned long int last_elem_index) {

	if (first_elem_index > last_elem_index) return;

	unsigned long int mid = (first_elem_index + last_elem_index) / 2;

	if (mid > first_elem_index) mergesort(tab, first_elem_index, mid);
	if (mid + 1 < last_elem_index) mergesort(tab, mid + 1, last_elem_index);
	merge(tab, first_elem_index, mid, last_elem_index);

}

/**************************************************************************************************************************
*
*                                                          INTROSORT
*
***************************************************************************************************************************/

void build_heap(vector<long int>& tab, int heapSize, int parentIndex, unsigned long int first_elem_index) {

	/*
	sortowana tablica jest fragmentem wiêkszej i ich pocz¹tki niekoniecznie musz¹ siê pokrywaæ,
	dlatego w ka¿dym odwo³aniu do jej elemnetu przesuwamy siê o wartoœæ pocz¹tkowego indeksu sortowanego fragmentu tablicy
	*/
	int maxIndex = parentIndex;
	int leftChild = parentIndex * 2 + 1;//wyliczenie wartosci indeksow prawego i lewego dziecka
	int rightChild = parentIndex * 2 + 2;

	if (leftChild <heapSize && tab[leftChild + first_elem_index] > tab[maxIndex + first_elem_index])//sprawdzamy czy dziecko jest wewn¹trz kopca i czy jest wiêksze od rodzica
		maxIndex = leftChild;
	if (rightChild <heapSize && tab[rightChild + first_elem_index] > tab[maxIndex + first_elem_index])//analogicznie dla prawego dziecka
		maxIndex = rightChild;
	if (maxIndex != parentIndex) {//jeœli wyst¹pi³o jakiekolwiek przesuniêcie zmieniamy wartoœæ korzenia i przebudowujemy kopiec
		swap(tab[maxIndex + first_elem_index], tab[parentIndex + first_elem_index]);
		build_heap(tab, heapSize, maxIndex, first_elem_index);
	}
}

void heapsort(vector<long int>& tab, unsigned long int first_elem_index, unsigned long int last_elem_index) {

	unsigned long int size = last_elem_index - first_elem_index + 1;

	for (int i = size / 2 - 1; i >= 0; i--) {//i - indeks ostatniego rodzica
		build_heap(tab, size, i, first_elem_index);
	}
	for (int i = size - 1; i > 0; i--) {
		swap(tab[first_elem_index], tab[i + first_elem_index]);
		build_heap(tab, --size, 0, first_elem_index);
	}
}

void introsort(vector<long int>& tab, unsigned long int first_elem_index, unsigned long int last_elem_index, int recursion_couter = 10) {

	if (recursion_couter <= 0) { // je¿eli liczba rekursji przekroczy okreœlon¹ wartoœæ, u¿yj heapsortu zamiast quicksortu
		heapsort(tab, first_elem_index, last_elem_index);
		return;
	}

	unsigned long int pivot_index;

	if (first_elem_index < last_elem_index) {
		pivot_index = split_array(tab, first_elem_index, last_elem_index);
		if (pivot_index != first_elem_index) introsort(tab, first_elem_index, pivot_index - 1, recursion_couter - 1);
		if (pivot_index != last_elem_index) introsort(tab, pivot_index + 1, last_elem_index, recursion_couter - 1);
	}
}

/**************************************************************************************************************************
*
*                                                            TEST
*
***************************************************************************************************************************/
bool test_sort(const vector<long int>& testing_tab, unsigned long int tab_size) {
	for (unsigned long int i = 0; i < tab_size; i++) {
		if (testing_tab[i + 1] < testing_tab[i]) {
			//cout << endl << "**************************TEST FAILED**************************" << endl;
			return false;
		}
	}
	//cout << endl << "**************************TEST PASSED**************************" << endl;
	return true;
}

void prepare_array(vector<long int>& arr, int option, unsigned long int size) {

	/*
	* PARAMETR "OPTION" ODPOWIADA ZA STOPIEÑ UPORZ¥DKOWANIA TABELI WEJŒCIOWEJ TZN W ZALE¯NOŒCI OD NIEGO ELEMENTY W TABLICY S¥ U£O¯ONE W CA£KOWICIE LOSOWEJ KOLEJNOŒCI,
	* POSORTOWANE W 25%, POSORTOWANE W 50% ITD, LUB POSORTOWANE W KOLEJNOŒCI MALEJ¥CEJ (CZYLI ODWRTOTNEJ DO SPOSOBU SORTOWANIA, WSZYSTKIE ALGORYTMY SORTUJ¥ ELEMENTY ROSN¥CO
	*/
	arr.clear();//wyczyœæ przyjêt¹ tablice
	float factor;
	unsigned long int random_number;

	switch (option)
	{
	case RAND:factor = 0; break;
	case SORTED_25:	factor = 0.25;	break;
	case SORTED_50:	factor = 0.5;	break;
	case SORTED_75:	factor = 0.75;	break;
	case SORTED_95:	factor = 0.95;	break;
	case SORTED_99:	factor = 0.99;	break;
	case SORTED_997:factor = 0.997;	break;
	case INVERTED_SORT:	factor = 1;	break;
	default: factor = 0; break;
	}

	if (option != INVERTED_SORT) {
		for (unsigned long int i = 0; i < size * factor; i++) arr.push_back(2);//wype³nienie uporz¹dkowane
		for (unsigned long int i = 0; i < size - (size * factor); i++) arr.push_back(rand());//dope³niene losowym
	}
	else {
		for (unsigned long int i = size; i > 0; i--) arr.push_back(i);//wype³nienie uporz¹dkowane
	}
}

string array_orderliness(const int &option) {
	switch (option)
	{
	case RAND:return "KOLEJNOSC LOSOWA"; break;
	case SORTED_25:	return "ELEMENTY UPORZADKOWANE W 25%";	break;
	case SORTED_50:	return "ELEMENTY UPORZADKOWANE W 50%";	break;
	case SORTED_75:	return "ELEMENTY UPORZADKOWANE W 75%";	break;
	case SORTED_95: return "ELEMENTY UPORZADKOWANE W 95%";	break;
	case SORTED_99:	return "ELEMENTY UPORZADKOWANE W 99%";	break;
	case SORTED_997:return "ELEMENTY UPORZADKOWANE W 99.7%";	break;
	case INVERTED_SORT:	return "ELEMENTY POSORTOWANE W ODWROTNEJ KOLEJNOSCI";	break;
	default: return "NIE ROZPOZNANO PRZYPADKU"; break;
	}
}


int main() {

	srand(currentTimeMillis() / 10000);//zmiana ziarna, ¿eby zapewniæ pseudolosowaæ za kazdym wywo³aniem programu

	fstream out_file;
	out_file.open("D:\\Studia AiR\\Sem 4\\PAMSI\\PAMSI_P1\\wyniki_testu.txt", ios::out | ios::in);

	vector<long int> array_of_rands;
	__int64 start_time = 0, finish_time = 0, sorting_time_quicksort = 0, sorting_time_mergesort = 0, sorting_time_introsort = 0;
	int number_of_iteriations = 100;
	vector<unsigned long int> array_size_options = { 10000, 50000, 100000, 500000, 1000000}; 
	vector<int> sorting_vector_types = {RAND, SORTED_25, SORTED_50 ,SORTED_75, SORTED_95, SORTED_99, SORTED_997, INVERTED_SORT};

	if (out_file.good()) {
		out_file << "CZAS LICZONO W us" << endl;
		out_file << "KOLEJNOSC CZASOW: QUICKSORT MERGESORT INTROSORT" << endl;
		for (const int& sorting_vector_type : sorting_vector_types) {//dla ró¿nego uporz¹dkowania wektora przed sortowaniem
			out_file << array_orderliness(sorting_vector_type) << endl;

			for (const auto& array_size : array_size_options) {//dla ró¿nych rozmairów tablic
				out_file << "ROZMIAR: " << array_size <<" " <<"CZAS: ";

				for (int i = 0; i < number_of_iteriations; ++i) {//przejœcie po 100 tablicach
					for (int sort_type = 1; sort_type <= 3; sort_type++) {//dla 3 ró¿nych rodzajów sortowania

						prepare_array(array_of_rands, sorting_vector_type, array_size);//generujemy wektor liczb losowych

						start_time = currentTimeMillis();

						if (sort_type == 1) quicksort(array_of_rands, 0, array_size);
						else if (sort_type == 2) mergesort(array_of_rands, 0, array_size);
						else if (sort_type == 3)introsort(array_of_rands, 0, array_size);

						finish_time = currentTimeMillis();

						if (sort_type == 1) sorting_time_quicksort += (finish_time - start_time);
						else if (sort_type == 2) sorting_time_mergesort += (finish_time - start_time);
						else if (sort_type == 3)sorting_time_introsort += (finish_time - start_time);

						if (!test_sort(array_of_rands, array_size)) {//walidacja sortowania
							std::cout << "ELEMENTY NIEPRAWIDOLO POSORTOWANE" << endl;
							return 0;
						}
					}
				}
				 
				out_file << sorting_time_quicksort / number_of_iteriations << " "
					<< sorting_time_mergesort / number_of_iteriations << " "
				    << sorting_time_introsort / number_of_iteriations <<  endl;
				sorting_time_introsort = 0;
				sorting_time_mergesort = 0;
				sorting_time_quicksort = 0;
				std::cout << "ZAKONCZONO SORTOWANIE TABLIC O ROZMIARZE: " << array_size << endl;
			}
			std::cout << endl << "ZAKONCZONO SORTOWANIE: " << array_orderliness(sorting_vector_type)  << endl << endl;
		}
	}
	else 
		std::cout << "NIE MOZNA OTWORZYC PLIKU" << endl; 
	
	return 0;
}
