#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <windows.h>
using namespace std;

    const int n = 31; // длина кодового слова n=q^m-1, q = 2 в данном примере.
    const int k = 27; // длина слова источника

// Это массив со значениями, соответствующими 2 в степени от 0 до 30
    vector<int> exp_ = {
         1, 2, 4, 8, 16, 5, 10, 20, 13, 26, 17, 7, 14, 28, 29, 31, 27, 19, 3, 6, 12, 24, 21, 15, 30, 25, 23, 11, 22, 9, 18
    };

// Массив, содержащий степени в которые нужно возвести 2, чтобы получить значение индекса. 2^0=1, 2^1=2, 2^4=3, ...
// Первый элемент массива зададим равным нулю.
    vector<int> log_ = {
         0, 0, 1, 18, 2, 5, 19, 11, 3, 29, 6, 27, 20, 8, 12, 23, 4, 10, 30, 17, 7, 22, 28, 26, 21, 25, 9, 16, 13, 14, 24, 15
    };


// multNumbers возвращает результат умножения чисел, передаваемых через параметры функции
int multNumbers(int x, int y)
{
    int mult;
    if (x == 0 or y == 0)
        return 0;
    mult = exp_[(log_[x] + log_[y])%n];
    return mult;
}

// divisionOfNumbers возвращает результат деления чисел, передаваемых через параметры функции
int divisionOfNumbers(int x, int y)
{
    int division;
    if (x == 0)
        return 0;

    if(y == 0) {
       cout << "Ошибка. Попытка реализовать деление на ноль" << endl;
       return -1;
    }

    division = exp_[((log_[x] - log_[y])+ n)%n];

    return division;
}

// multiplyByXPower(vector<int> vec, unsigned short power) возвращает результат умножения многочлена
//на х^t, где t передаётся через параметры функции
vector<int> multiplyByXPower(vector<int> vec, unsigned short power)
{
    reverse(vec.begin(), vec.end());
    for (unsigned short i = 0; i < power; i++)
        vec.push_back(0);

    reverse(vec.begin(), vec.end());

    return vec;
}

// multiplyPolyByNum возвращает произведение многочлена на число.
vector<int> multiplyPolyByNum(vector<int> poly, int number)
{
    vector<int> newVec (poly.size(), 0);

    for (unsigned short i = 0; i < poly.size(); i++)
        if(poly[i] != 0 and number != 0)
            newVec[i] = multNumbers(poly[i], number);

    return newVec;
}

// sumOfPolynomials возвращает сумму многочленов.
vector<int> sumOfPolynomials(vector<int> firstPoly, vector<int> secondPoly)
{
    unsigned short maxSize = 0; // Чтобы определить длину большего вектора, иначе говоря, степень большего полинома
    maxSize = max(firstPoly.size(), secondPoly.size());
    vector<int> sum (maxSize, 0);

    // Приводим векторы к одному размеру, чтобы легче было складывать
    if (firstPoly.size()!= maxSize)
        for (unsigned short i = 0; i < (maxSize - firstPoly.size()); i++)
            firstPoly.push_back(0);
    else
        for (unsigned short i = 0; i < (maxSize - firstPoly.size()); i++)
            secondPoly.push_back(0);

    for (unsigned short i = 0; i < maxSize; i++)
        sum[i] = firstPoly[i] ^ secondPoly[i];

    return sum;
}

// multOfPolynomials возвращает произведение многочленов
vector<int> multOfPolynomials(vector<int> FirstPoly, vector<int> SecondPoly)
{
    vector<int> result((FirstPoly.size() + SecondPoly.size() - 1), 0);

    // Ниже каждый коэффициент FirstPoly умножается на каждый коэффициент SecondPoly
    for (int j = 0; j < SecondPoly.size(); j++)
        for (int i = 0; i < FirstPoly.size(); i++)
            result[i + j] ^= multNumbers(FirstPoly[i], SecondPoly[j]);

    return result;
}


 // ExponentiationOfNum(int num, int degree) возведение числа в степень
int ExponentiationOfNum(int num, int degree)
{
    int result;
    if(log_[num]==0)
        result = exp_[0];
    else if(degree < 0)
             result = exp_[n - ((log_[num] * degree*(-1))%n)];
         else
             result = exp_[(log_[num] * degree)%n];
    return result;
}


// findZerosOfPoly(vector<int> poly) возвращает вектор с корнями многочлена, переданного по параметру.
vector<int> findZerosOfPoly(vector<int> poly)
{
    vector<int> zeros;
    int tempValue = poly[0];

    for (unsigned short i = 0; i < exp_.size(); i++)
    {
        for (unsigned short j = 1; j < poly.size(); j++)
            tempValue ^= multNumbers(poly[j],ExponentiationOfNum(exp_[i],j));

        if(tempValue==0)
            zeros.push_back(exp_[i]);
        tempValue = poly[0];

        if(zeros.size() == (poly.size()-1))
            break;
    }

    return zeros;
}

// Алгоритм Берлекэмпа-Мэсси для поиска полинома локаторов ошибок. Функция выводит false, если ошибки в искажённом кодовом
// слове не исправить, и true, если ошибки исправить можно. В последнем случае полином локаторов ошибок записывается в
// передаваемый в параметрах функции вектор desiredLocator.
bool CalcLocatorPoly(vector<int> syndromes, vector<int>& desiredLocator)
{
    bool errorsCanBeFixed = true;
    unsigned short L = 0, delta = 0;// Текущая длина регистра
	vector<int> currentLocator, previousLocator, newLocator, multPrevLocatorByX; // Текущий, предыдущий и новый многочлен локаторов ошибок

	currentLocator.push_back(1); // currentLocator = 1 или 1*(х^0)
	previousLocator.push_back(1);

	for (unsigned short i = 1; i <= syndromes.size(); i++)
    {
        delta = syndromes[i-1];
		for (unsigned short j = 1; j <= L; j++)
            delta ^= multNumbers(currentLocator[j], syndromes[i - 1 - j]);

        multPrevLocatorByX = multiplyByXPower(previousLocator, 1); //Умножение полинома на икс (эквивалентно сдвигу вправо на 1 байт)

		if (delta != 0)
        {
            newLocator = sumOfPolynomials(currentLocator, multiplyPolyByNum(multPrevLocatorByX, delta));
			if (2*L <= i-1)
			{
				previousLocator = multiplyPolyByNum(currentLocator, ExponentiationOfNum(delta, -1));
				L = i - L;
			}
			else
                previousLocator = multPrevLocatorByX;
			currentLocator = newLocator;
		}
	}

	if ((currentLocator.size()-1) != L)
	{
        //cout << "В полученном искажённом кодовом слове слишком много ошибок" << endl;
        errorsCanBeFixed = false;
        return errorsCanBeFixed;
	}

	desiredLocator = currentLocator;
	return errorsCanBeFixed;
}

// Умножение вектора на матрицу
vector<int> multVectorByMatrix(vector<int> vec, vector <vector <int>> matrix, int numOfColMatrix, int numOfLinesMatrix)
{
    vector<int> resVector(numOfColMatrix,0), temp_vec(numOfLinesMatrix,0);
    // temp_vec - временный вектор, в котором будут храниться произведения пар элементов msg на столбец
    // матрицы G, в следующем виде: temp_vec[0]=msg[0]*G[0][j], temp_vec[1]=msg[1]*G[1][j], ... . j меняется от 0 до n-1.

		    for (unsigned short j = 0; j < numOfColMatrix; j++)
		    {
                for (unsigned short i = 0; i < numOfLinesMatrix; i++)
                    temp_vec[i] = multNumbers(vec[i], matrix[i][j]);

		        // Складываем двоичные представления элементов вектора temp_vec, результат переводим в 10-ую систему счисления
		        // и в результате получаем i-тый символ кодового слова
		        for(unsigned short s = 0; s < temp_vec.size(); s++)
                    resVector[j] ^= temp_vec[s];

		        fill(temp_vec.begin(), temp_vec.end(), 0); // Зануляем вектор temp_vec binarySum
		    }

    return resVector;
}

// findErrorEvaluatorPoly возвращает полином для оценки ошибок
vector<int> findErrorEvaluatorPoly(vector<int> syndrome, vector<int> locator, int r)
{
	vector<int> multOfPoly, errorEvaluatorPoly;

	multOfPoly = multOfPolynomials(syndrome, locator);

	for (int i = 0; i < r; i++) // (syndrome * locator) mod x^(r)
		errorEvaluatorPoly.push_back(multOfPoly[i]);

    for (int i = 0; i < errorEvaluatorPoly.size(); i++) // вектора вида 10 2 0 0 приводятся к виду 10 2
		if(errorEvaluatorPoly[errorEvaluatorPoly.size()-1] == 0)
            errorEvaluatorPoly.pop_back();

	return errorEvaluatorPoly;
}

// findDerivativeOfPoly возвращает производную многочлена
vector<int> findDerivativeOfPoly(vector<int> poly)
{
    vector<int> derivativeOfPoly(poly.size()-1, 0);

    for (int i = 1; i < poly.size(); i = i + 2)
        if (poly[i] != 0)
            derivativeOfPoly[i-1] = poly[i];

    for (int i = 0; i < derivativeOfPoly.size(); i++) // вектора вида 10 2 0 0 приводятся к виду 10 2
		if(derivativeOfPoly[derivativeOfPoly.size()-1] == 0)
            derivativeOfPoly.pop_back();


    return derivativeOfPoly;
}


// valueOfPolyAtThePoint возвращает значение многочлена от известного x
int valueOfPolyAtThePoint(vector<int> poly, int x)
{
    int result = poly[0];

    for (int i = 1; i < poly.size(); i++)
        result ^= multNumbers(poly[i], ExponentiationOfNum(x,i));

    return result;
}


// findValueOfErrors возвращает вектор со значениями ошибок, допущенных в искажённом кодовом слове.
vector<int> findValueOfErrors(vector<int> errorEvaluatorPoly, vector<int> derivativeOfLocator, vector<int> zerosOfLocator)
{
    vector<int> valueOfErrors(zerosOfLocator.size());

    for (int i = 0; i < zerosOfLocator.size(); i++)
         valueOfErrors[i] = divisionOfNumbers(valueOfPolyAtThePoint(errorEvaluatorPoly, zerosOfLocator[i]),
                                             valueOfPolyAtThePoint(derivativeOfLocator, zerosOfLocator[i]));

    return valueOfErrors;
}

// errorPoly возвращает полином, который способен исправить ошибки в искажённом кодовом слове.
vector<int> findErrorPoly(vector<int> valueOfErrors, vector<int> revZerosOfLocator)
{
    vector<int> errorPoly(n, 0);

    for (int i = 0; i < revZerosOfLocator.size(); i++)
        errorPoly[log_[revZerosOfLocator[i]]] = valueOfErrors[i];

    return errorPoly;
}

// fixDisCodeword возвращает исправленное кодовое слово, если его можно исправить.
vector<int> fixDisCodeword(vector<int> disCodeword, vector<int> errorPoly)
{
    vector<int> correctCodeword(n, 0);

    for (int i = 0; i < correctCodeword.size(); i++)
        correctCodeword[i] = disCodeword[i] ^ errorPoly[i];

    return correctCodeword;
}

vector<int> serchAndFixErrors(vector<int> syndrom, vector<int> someCodeword, int r)
{
    vector<int> locator; // Вектор, представляющий из себя ногочлен локаторов ошибок в искажённом кодовом слове
    vector<int> zerosOfLocator; // Вектор с корнями полинома локаторов ошибок
    vector<int> revZerosOfLocator; // Вектор с элементами, обратными к корням полинома локаторов ошибок
    bool locatorIsFound;
    vector<int> correctCodeword; // Исправленное кодовое слово
    vector<int> errorEvalPoly; // Омега(x)
    vector<int> derivativeOfLocator; // Производная лямбда(x)
    vector<int> valueOfErrors; // Вектор со значениями ошибок в искажённом кодовом слове
    vector<int> polyToFixCodeword; // Полином для исправления ошибок в искажённом кодовом слове



    locatorIsFound = CalcLocatorPoly(syndrom, locator);

    if(locatorIsFound == false)
        return correctCodeword;


    zerosOfLocator = findZerosOfPoly(locator);


    for (int i = 0; i < zerosOfLocator.size(); i++)
        revZerosOfLocator.push_back(ExponentiationOfNum(zerosOfLocator[i], -1));

    errorEvalPoly = findErrorEvaluatorPoly(syndrom, locator, r);

    derivativeOfLocator = findDerivativeOfPoly(locator);

    valueOfErrors = findValueOfErrors(errorEvalPoly, derivativeOfLocator, zerosOfLocator);

    polyToFixCodeword = findErrorPoly(valueOfErrors, revZerosOfLocator);

    correctCodeword = fixDisCodeword(someCodeword, polyToFixCodeword);

    return correctCodeword;
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int r = n-k; // количество избыточных символов
    int d = r+1; // Минимальное расстояние Хэмминга
    //const int m = 5; // Это степень неприводимого многочлена
// 2=00010=x - это альфа, то есть примитивный элемент данного поля Галуа.

// Неприводимый многочлен равен x^5+x^2+1 = 101001 vector <int> irreduciblePoly = {1,0,1,0,0,1};

// Порождающий многочлен g(x)= x^4+30x^3+6x^2+9x+17
    vector <int> generetedPoly = {17,9,6,30,1};

    vector<int> encoded_message; // Закодированное сообщение
    encoded_message.reserve(n);

    vector <vector <int>> G(k, vector <int> (n)); // Порождающая матрица
    vector <vector <int>> H(n, vector <int> (r)); // Проверочная матрица
    vector <vector <int>> decodingMatrix(n, vector <int> (k)); // Для декодирования кодовых слов
    vector<char> alphabet = {' ', 'а', 'б', 'в', 'г', 'д', 'е', 'ж', 'з', 'и', 'й', 'к', 'л', 'м', 'н', 'о', 'п',
    'р', 'с', 'т', 'у', 'ф', 'х', 'ц', 'ч', 'ш', 'щ', 'ь', 'ы', 'э', 'ю', 'я'};
    string symbolicMessage; // Символьное сообщение, вводимое с консоли
    vector<int> message(k); // message - слово источника, состоящее из элементов поля Галуа
    string DecodedSymbMessage; // Декодированное сообщение, переведённое в символы
    vector<int> locator; // Вектор, представляющий из себя ногочлен локаторов ошибок в искажённом кодовом слове
    string stringCodeword; // Для записи с консоли кодового слова, которе предстоит декодировать
    vector<int> someCodeword; // Кодовое слово stringCodeword переведённое в тип int
    int valueCodeword = 0; // Для преобразования stringCodeword в вектор типа int
    bool syndromOfZeros = true; // Для проверки на наличие ошибок в введённом с консоли кодовом слове
    vector<int> decodedMessage; // Декодированное сообщение, состоящее из символов поля Галуа
    vector<int> correctCodeword; // Исправленное кодовое слово
    vector<int> syndrom; // Синдром
    vector<int> zerosOfGenPoly; // Корни порождающего многочлена

    // Задаём порождающую матрицу (2 конструкция кода РС)
    for (unsigned short i = 0; i < k; i++) // кол-во строк
        for (unsigned short j = 0; j < n; j++) // кол-во столбцов
            G[i][j] = ExponentiationOfNum(exp_[j],i);

    // Задаём матрицу для декодирования (все элементы возводятся в -1 степень)
    for (unsigned short i = 0; i < n; i++)
        for (unsigned short j = 0; j < k; j++)
            decodingMatrix[i][j] = ExponentiationOfNum(exp_[j], n-i);


    // Находим корни порождающего многочлена для построения проверочной матрицы
    zerosOfGenPoly = findZerosOfPoly(generetedPoly);


    // Задаём проверочную матрицу (2 конструкция кода РС)
    for (unsigned short i = 0; i < n; i++)
        for (unsigned short j = 0; j < r; j++)
            H[i][j] = ExponentiationOfNum(zerosOfGenPoly[j],i);



newSymMessage:
    cout << "Введите сообщение длины 27, используя символы русского алфавита (за исключением ё и ъ) и пробел:\n";
    getline(cin, symbolicMessage);

    if (symbolicMessage.size() < k)
        for (int i = k - symbolicMessage.size(); i > 0; i--)
            symbolicMessage.push_back(' ');
    else if(symbolicMessage.size() > k)
        {
            cout << "\nРазмер введённого сообщения превышает допустимый\n\n";
            goto newSymMessage;
        }

    // Переводим символьное сообщение в слово источника с элементами из поля Галуа
    for (int i = 0; i < symbolicMessage.size(); i++)
        for (int j = 0; j < alphabet.size(); j++)
            if(symbolicMessage[i] == alphabet[j])
               message[i] = j;


    // Выводим слово источника с элементами из поля Галуа
    cout << "\nВаше слово:\n";
    for (auto x : message)
        cout << x << " ";

    // Кодируем слово источника
    cout << "\nКодовое слово:\n";
    encoded_message = multVectorByMatrix(message, G, n, k);
    for (int i = 0; i < encoded_message.size(); i++)
         cout << encoded_message[i] << " ";
    cout << endl;


    // someCodeword - кодовое слово, которое вводит пользователь. Потенциально искажённое.
newStrCodeword:
    cout << "\nВведите слово длины 31, которое хотите декодировать:\n";
    getline(cin, stringCodeword);

    // ввод с консоли кодового слова для дальнейшего его декодирования
    for (int i = 0; i < stringCodeword.size()-1; i++)
    {
        if(stringCodeword[i] != ' ' & stringCodeword[i+1] != ' ')
        {
            valueCodeword = ((int)stringCodeword[i] - '0')*10;
            if(i == stringCodeword.size()- 2)
            {
                valueCodeword += (int)stringCodeword[i+1]- '0';
                someCodeword.push_back(valueCodeword);
            }
        }
        else if (stringCodeword[i] != ' ' & stringCodeword[i+1] == ' ') {
            valueCodeword += (int)stringCodeword[i]- '0';
            someCodeword.push_back(valueCodeword);
            valueCodeword = 0;
        }

        if(i == stringCodeword.size()- 2 & stringCodeword[i] == ' ' & stringCodeword[i+1] != ' ')
            {
                valueCodeword = (int)stringCodeword[i+1]- '0';
                someCodeword.push_back(valueCodeword);
            }
    }


    if (someCodeword.size() != n) {
        cout << "\nРазмер введённого кодового слова не равен 31\n\n";
        someCodeword.clear();
        goto newStrCodeword;
    }


    // Считаем синдром введённого кодового слова
    syndrom = multVectorByMatrix(someCodeword, H, r, n);


    // Проверяем, является ли синдром введённого с консоли кодового слова нулевым
    for (int i = 0; i < syndrom.size(); i++)
         if(syndrom[i]!=0)
         {
             syndromOfZeros = false;
             break;
         }

  if(syndromOfZeros == false)
  {
    correctCodeword = serchAndFixErrors(syndrom, someCodeword, r);

    if (!correctCodeword.empty())
    {

    cout << "\nКодовое слово (после исправления ошибок):\n";
    for (int i = 0; i < correctCodeword.size(); i++)
        cout << correctCodeword[i] << " ";

    // Декодируем сообщение
    decodedMessage = multVectorByMatrix(correctCodeword, decodingMatrix, k, n);
    cout << "\nДекодированное сообщение:\n";
    for (int i = 0; i < decodedMessage.size(); i++)
         cout << decodedMessage[i] << " ";

    // Переводим сообщений на символьный язык
    for (int i = 0; i < decodedMessage.size(); i++)
         DecodedSymbMessage.push_back(alphabet[decodedMessage[i]]);

    cout << "\nДекодированное символьное сообщение:\n";
    for (int i = 0; i < DecodedSymbMessage.size(); i++)
         cout << DecodedSymbMessage[i];
    cout << endl;

    }
    else if (correctCodeword.empty())
    {
        cout << "В введённом кодовом слове ошибки не могут быть исправлены.";
    }
  }
  else {
    correctCodeword = someCodeword;

    // Декодируем сообщение
    decodedMessage = multVectorByMatrix(correctCodeword, decodingMatrix, k, n);
    cout << "Декодированное сообщение:\t";
    for (int i = 0; i < decodedMessage.size(); i++)
         cout << decodedMessage[i] << " ";
    cout << endl;

    // Переводим сообщения на символьный язык
    for (int i = 0; i < decodedMessage.size(); i++)
         DecodedSymbMessage.push_back(alphabet[decodedMessage[i]]);

    cout << "Декодированное символьное сообщение:\t";
    for (int i = 0; i < DecodedSymbMessage.size(); i++)
         cout << DecodedSymbMessage[i];
    cout << endl;
  }


    return 0;
}
