import telebot

from config import keys, TOKEN

from extensions import ConversionException, CurrencyConverter

bot = telebot.TeleBot(TOKEN)

@bot.message_handler(commands=['start', 'help'])
def echo_test(message: telebot.types.Message):
    text = 'Чтобы начать работу введите команду боту в следующем формате:\n<название валюты> \
<в какую валюту перевести> \
<количество переводимой валюты>.\nПоказать список доступных валют: /values'
    bot.reply_to(message, text)


@bot.message_handler(commands=['values'])
def echo_test(message: telebot.types.Message):
    text = 'Доступные валюты:'
    for key in keys.keys():
        text = '\n'.join((text, key,))
    bot.reply_to(message, text)


@bot.message_handler(content_types=['text'])
def convert(message: telebot.types.Message):
    try:
        values = message.text.split(' ')

        if len(values) < 3:
            raise ConversionException('Введено слишком мало параметров!')
        if len(values) > 3:
            raise ConversionException('Введено слишком много параметров!')

        quote, base, amount = values
        result = CurrencyConverter.convert(quote, base, amount)
    except ConversionException as e:
        bot.reply_to(message, f'Ошибка пользователя {e}')
    except Exception as e:
        bot.reply_to(message, f'Не удалось обработать команду {e}')
    else:
        text = f'Цена {amount} {quote} = {result} {base}'
        bot.send_message(message.chat.id, text)

bot.polling()