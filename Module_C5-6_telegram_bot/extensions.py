import requests
import json

from config import keys, APIKEY

class ConversionException(Exception):
    pass

class CurrencyConverter:
    @staticmethod
    def convert(quote: str, base: str, amount: str):
        if quote not in keys.keys():
            raise ConversionException('при вводе названия первой валюты.')

        if base not in keys.keys():
            raise ConversionException('при вводе названия второй валюты.')

        try:
            f_amount = float(amount)
        except ValueError:
            raise ConversionException('- невозможно обработать указанное количество.')

        if f_amount <= 0:
            raise ConversionException('- введённое количество не может быть меньше или равно нулю.')

        if quote == base:
            raise ConversionException(f'- невозможно перевести одинаковые вылюты: {base}.')

        query = keys[quote] + '_' + keys[base]  # формирование текста запроса из кодов валют
        url = 'https://free.currconv.com/api/v7/convert?q=' + query + '&compact=ultra&apiKey=' + APIKEY
        r = requests.get(url)   # получение запроса с сервера

        if r == None:
            raise Exception('- ошибка сервера.')

        try:
            # data = json.loads(r.content)
            string = str(r.content)
            string = string[string.find(':') + 1:-2:]   # извлечение валютного курса из полученной строки

        #except JSONDecodeError("Неизвестная ошибка получения курса валют."):
        except:
            raise Exception('- неизвестная ошибка получения курса валют.')

        try:
            value = float(string)
        except:
            raise Exception('- неизвестная ошибка преобразования курса валют.')   # возникает при команде "эфириум рубль 1"
        else:
            return value * f_amount
