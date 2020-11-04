#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std;

enum type_error
{
	ERROR_ALLOCATION_MEMORY,   
	SUCCESS_ALLOCATION_MEMORY,

	ERROR_OPEN_STREAM,
	ERROR_INCORRECT_ELEMENT,
	ERROR_RECTANGLE_MATRIX,
	ERROR_NO_MATRIX,
	SUCCESS_LOAD_MATRIX,

	ERROR_PROCESSING_MATRIX,
	SUCCESS_PROCESSING_MATRIX
};

void free_memory(int **&matrix, int row)//--очистка памяти
{
	if (matrix == NULL) return;//-----------если матрица пустая то завершить функцию 

	for (int i = 0; i < row; i++)//---------перебор элементов
		delete[] matrix[i];//---------------удаление текущего элемента

	delete[] matrix;//----------------------удаление массива

	matrix = NULL;//------------------------присвоение нулевого указателя. NULL-макрос препроцессора
}

type_error aloc_mem(int **&matrix, int row, int col, int *error_row = NULL)//-- выделение памяти
{
	matrix = new(nothrow) int*[row];

	if (matrix == NULL) return ERROR_ALLOCATION_MEMORY;

	for (int i = 0; i < row; i++)
	{
		matrix[i] = new(nothrow) int[col];

		if (matrix[i] == NULL)
		{
			if (error_row != NULL) error_row = &i;

			free_memory(matrix, i);
			return ERROR_ALLOCATION_MEMORY;
		}
	}

	return SUCCESS_ALLOCATION_MEMORY;
}

type_error correct_element(const string filename, int **&matrix, int &row, int &col, string &error)
{
	fstream stream;

	stream.open(filename, ios::in);//--открываем файл в поток. ios::in-чтение

	if (!stream.is_open())//-----------проверка открытия файла
	{
		row = 0;
		col = 0;
		return ERROR_OPEN_STREAM;
	}

	int a;
	int m = 0, n = 0;//-- m — колонка, n — строчка 9999999999
	int k = 0;

	int index = 0;
	char c = 0;

	char lseek;
	bool b = true;

	while (b)
	{
		stream >> ws;//-------------удаление начальных пробелов

		if (stream.eof()) break;//--проверка конца файла

		index = stream.tellg();//---запись текущей позиции чтения
		stream >> a;//--------------извлекаем символ из потока
		lseek = stream.peek();//----смотрим на следуещий символ в потоке

		if ((stream.fail() && !stream.eof()) || (lseek != ' ' && lseek != '\t' && lseek != '\n' && lseek != EOF))
		{   //                                                                            --fail() — возвращает true, если установлен failbit (значит, что произошла какая-то не фатальная ошибка);
			stream.clear();              //сбрасываем все флаги ошибок                    --eof() — возвращает true, если установлен eofbit (значит, что поток находится в конце файла);
			stream.seekg(index) >> error;//устанавливает абсолютную позицию чтения-       --' ' — пробел, '\t' — горизонтальная табуляция, '\n' — новая строка, EOF — конец файла
			                             //и извлекаем ошибочный элемент 
			stream.close();              //закрываем поток
			//                              
			row = n;
			col = m;

			return ERROR_INCORRECT_ELEMENT;
		}

		m++;

		while (c != EOF)
		{
			c = stream.get();

			switch (c)
			{
			case ' ': case '\t':	break;
			case EOF: b = false;
			case '\n':
			{
				c = EOF;

				if (k != m * n)
				{
					stream.close();

					row = n;
					return ERROR_RECTANGLE_MATRIX;
				}

				n++;
				k += m;
				m = 0;
				break;
			}
			default:
			{
				stream.unget();
				c = EOF;
			}
			}
		}

		c = 0;
	}

	if (!n)
	{
		row = 0;
		col = 0;
		return ERROR_NO_MATRIX;
	}

	m = k / n;

	if (aloc_mem(matrix, n, m, &row) == ERROR_ALLOCATION_MEMORY)
	{
		return ERROR_ALLOCATION_MEMORY;
	}

	stream.clear();
	stream.seekg(0);

	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			stream >> matrix[i][j];

	row = n;
	col = m;

	stream.close();
	return SUCCESS_LOAD_MATRIX;
}

void matrix_console(int **matrix, int row, int col, const int width = 5)//--вывод матрецы
{
	if (matrix == NULL) return;

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			cout << setw(width) << matrix[i][j]; //*(*(matrix + i) + j);
		}

		cout << endl;
	}
}

void matrix_copy(int **&matrix, int **matrix_source, int col, int row)
{
	if (matrix == NULL || matrix_source == NULL) return;

	for (int i = 0; i < row; i++)
		for (int j = 0; j < col; j++)
			matrix[i][j] = matrix_source[i][j];
}

type_error matrix_processing(int **&matrix, int row, int col, int str)
{
	if (str < 0 || matrix == NULL) return ERROR_PROCESSING_MATRIX;

	int com = 1, max = 0;

	for (int i = 0; i < col; i++) {
		for (int a = 0; a < row; ) {
			com = com * matrix[a][i];
			//cout << com <<endl;
			if (max < matrix[a][i])
			{
				max = matrix[a][i];
			}
			a++;
			if ( a == row)
			{
				if (com < str)
				{
					for (int a = 0; a < row; a++) {
						if (max != matrix[a][i])
						{
							matrix[a][i] += max;
						}
					}

				}
			}
		}
		max = 0;
		com = 1;
	}
	

	return SUCCESS_PROCESSING_MATRIX;
}

int main()
{
	setlocale(LC_ALL, "rus");

	fstream stream;
	string filename;
	string error;
	type_error  errors;

	int **math_2 = NULL;
	int **math_2_copy = NULL;

	int row, col;

	cout << "Введите имя файла: ";
	getline(cin, filename);
	errors = correct_element(filename, math_2, row, col, error);

	switch (errors)
	{
	case ERROR_ALLOCATION_MEMORY:
	{
		cout << "Ошибка выделения памяти\nСтрока: " << row + 1 << endl;
		system("pause");
		return 0;
	}
	case ERROR_OPEN_STREAM:
	{
		cout << "Ошибка открытия потока" << endl;
		system("pause");
		return 0;
	}
	case ERROR_INCORRECT_ELEMENT:
	{
		cout << "Ошибка: некоректный элемент!\nСообщение: '" << error << "' [" << row + 1 << "][" << col + 1 << "]" << endl;
		system("pause");
		return 0;
	}
	case ERROR_RECTANGLE_MATRIX:
	{
		cout << "Ошибка: матрица не прямоугольная\nСтрока: " << row + 1 << endl;
		system("pause");
		return 0;
	}
	case ERROR_NO_MATRIX:
	{
		cout << "Ошибка: Файл не содержит матрицу!" << endl;
		system("pause");
		return 0;
	}
	}


	cout << "Исходная матрица:" << endl;
	matrix_console(math_2, row, col);

	if (aloc_mem(math_2_copy, row, col) == SUCCESS_ALLOCATION_MEMORY)
	{
		matrix_copy(math_2_copy, math_2, row, col);

		int stroka, stolbec;

		do
		{
			cout << "Задайте строку и столбцев: ";
			cin >> stroka;

			if (cin.peek() == 10) break;

			cin.clear();
			cin.ignore(cin.rdbuf()->in_avail());
		} while (true);


		if (matrix_processing(math_2_copy, row, col, stroka) == SUCCESS_PROCESSING_MATRIX)
		{
			cout << "Новая матрица:" << endl;
			matrix_console(math_2_copy, row, col);
		}
		else
		{
			if (matrix_processing(math_2_copy, row, col, stroka) == ERROR_PROCESSING_MATRIX)
			{
				cout << "Ошибка: некоректные данные!" << endl;
			}
		}
	}

	free_memory(math_2, row);
	free_memory(math_2_copy, row);

	system("pause");
	return 0;
}