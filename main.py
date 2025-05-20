# This is a sample Python script.
import json
from random import randint
import os
import random
import glob
from filecmp import cmp


def open_main_file():
    data = []
    # Список с названием файла из папки in
    filename = glob.glob('in/*.json')
    # data хранит содержимое этих файлов
    with open(filename[0], 'r') as file:
        data = json.load(file)
    return data


def is_prime(n):
    if n % 2 == 0:
        return n == 2
    d = 3
    while d * d <= n and n % d != 0:
        d += 2
    return d * d > n


# Функция poly создаёт полином
def poly(data, coefficients, x):
    y = data['secret']
    for i in range(data['k']-1):
        y = y + coefficients[i] * (x ** (i+1))
    return y % data['p']

# Функция create_fraction создаёт доли.
def create_fraction(x, y, data):
    new_fraction = {}
    new_fraction['x'] = x
    new_fraction['y'] = y
    new_fraction['p'] = data['p']
    new_fraction['n'] = data['n']
    new_fraction['k'] = data['k']
    with open('out/Fraction' + str(x) + '.json', 'w') as file:
        json.dump(new_fraction, file, indent=3)
    return


def create_file_with_secret(arr_data):
    file_with_secret = {}
    file_with_secret['secret'] = found_secret(arr_data)
    file_with_secret['p'] = arr_data[0]['p']
    file_with_secret['n'] = arr_data[0]['n']
    file_with_secret['k'] = arr_data[0]['k']
    with open('out/File_With_Secret.json', 'w') as file:
        json.dump(file_with_secret, file, indent=3)
    return


def secret_sharing():
    data = open_main_file()
    if not is_prime(data['p']):
        print('Error: p is not a prime number')
        return
    if data['secret'] > data['p']:
        print('Error: secret > p')
        return
    if data['k'] > data['n']:
        print('Error: k > n')
        return
    # Лист с рандомными значениями для полинома:
    coefficients = [randint(1, data['p'] - 1) for _ in range(data['k'] - 1)]
    # print(coefficients)

    for i in range(data['n']):
        create_fraction(i + 1, poly(data, coefficients, i + 1), data)

    print('Секрет разделён. Результат представлен в каталоге out.')
    return


"""
Функция revers_number_mod_p(p2, arr_data) вычисляет значение дробного числа по модулю p. Расширенный алгоритм Евклида.
"""
def revers_number_mod_p(den, arr_data):
    for k in range(1, arr_data[0]['p']):
        if ((den*k) % arr_data[0]['p']) == 1:
            return k


def found_secret(arr_data):
    sum = 0
    for j in range(len(arr_data)):
        num = 1   # num - числитель
        den = 1   # den - знаменатель
        for i in range(len(arr_data)):
            if j != i:
                num = num*(0 - arr_data[i]['x'])
                den = den*(arr_data[j]['x'] - arr_data[i]['x'])
            else:
                num = num*1
                den = den*1
        sum = sum+arr_data[j]['y']*(num * revers_number_mod_p(den, arr_data))
    return sum % arr_data[0]['p']


def reconstruct_secret():
    # Проверки:
    data = []
    # Список с названиями всех файлов из папки in
    filename_list = glob.glob('in/*.json')
    # data хранит содержимое этих файлов
    for filename in filename_list:
        with open(filename, 'r') as f:
            data.append(json.load(f))


    if len(filename_list) == 0:
        print('Ошибка: в папке in нет файлов.')
        return

    for i in range(1, len(data)):
        if data[0]['p'] != data[i]['p']:
            print('Ошибка: значение р в файлах разное.')
            return
        if data[0]['n'] != data[i]['n']:
            print('Ошибка: значение n в файлах разное.')
            return
        if data[0]['k'] != data[i]['k']:
            print('Ошибка: значение k в файлах разное.')
            return

    if len(filename_list) < data[0]['k']:
        print('Ошибка: количество файлов в папке in меньше значения k. Восстановить секрет не удастся.')
        return

    for i in range(len(data)):
        if data[i]['x'] > data[0]['p']:
            print('Ошибка: значение x в некоторых файлах больше значения р.')
            return
        if data[i]['y'] > data[0]['p']:
            print('Ошибка: значение y в некоторых файлах больше значения р.')
            return


    for i in filename_list:
        for j in filename_list:
            if i != j:    # Функция cmp сравнивает файлы по содержанию и выдаёт True, если они одинаковые
                if_dupl = cmp(
                    i,
                    j,
                    shallow=True
                )
                if if_dupl:
                    #  Убираем файлы с одинаковым содержанием из рассмотрения
                    filename_list.remove(str(j))

    if len(filename_list) < data[0]['k']:
        print('Ошибка: количество различных файлов в папке in меньше значения k. Восстановить секрет не удастся.')
        return

  # Выполняется, если все проверки пройдены
  #   Перезаписываем data на основе изменённого filename_list
    data.clear()
    for filename in filename_list:
        with open(filename, 'r') as f:
            data.append(json.load(f))

# Берём к рандомных разных файлов из папки in и на их основе восстанавливаем секрет
    random_num = random.sample(range(len(filename_list)), data[0]['k'])
    arr_data = [] # Массив со значениями из к json'овских файлов
    for i in range(len(random_num)):
        with open(filename_list[random_num[i]], 'r') as file:
            arr_data.append(json.load(file))

    create_file_with_secret(arr_data)
    print('Секрет восстановлен. Результат представлен в каталоге out.')
    return


def main():
    # """Main function"""
    # Очищаем папку out
    filelist = [f for f in os.listdir('out') if f.endswith(".json")]
    for f in filelist:
        os.remove(os.path.join('out', f))

    print('Выберите')
    print('-s для разделения секрета ')
    print('-r для восстановления секрета ')
    wish = input()
    if wish == '-s':
        secret_sharing()
    if wish == '-r':
        reconstruct_secret()

# Очищаем папку in
    filelist = [f for f in os.listdir('in') if f.endswith(".json")]
    for f in filelist:
        os.remove(os.path.join('in', f))

    input()

if __name__ == '__main__':
    main()
# See PyCharm help at https://www.jetbrains.com/help/pycharm/
