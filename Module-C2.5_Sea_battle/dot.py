import random as rnd


class Dot:
    def __init__(self, x = 0, y = 0):
       self.x = x
       self.y = y

    def __eq__(self, _dot):
        return (self.x == _dot.x) and (self.y == _dot.y)

    def set_xy(self, _x, _y):
        self.x = _x
        self.y = _y

    def get_input(self, max_x, max_y):
        while True:
            string = input(str("Введите ваши координаты: "))
            if string == "":
                continue
            string = string.strip()
            string = string.replace(',', ' ')
            string = string.replace('.', ' ')

            while '  ' in string:  # удаление лишних пробелов
                string = string.replace('  ', ' ')

            crd_list = string.split(' ')  # получение списка координат

            try:    # попытка получения координиаты x
                self.x = int(crd_list[0]) - 1  # перевод значения в координаты поля
            except ValueError:
                print("Ошибка ввода первой координаты! Повторите ввод.")

            try:    # попытка получения координиаты y
                self.y = int(crd_list[1]) - 1  # перевод значения в координаты поля
            except ValueError:
                print("Ошибка ввода второй координаты! Повторите ввод.")
                continue

            if self.x < 0 or self.y < 0:
                print("Ошибка ввода - координаты не могут быть отрицательными или нулевыми! Повторите ввод.")
                continue

            if self.x >= max_x or self.y >= max_y:
                print("Ошибка ввода - координаты превышают размер игрового поля! Повторите ввод.")
                continue
            else:
                break

    def get_random_dot(self, min_x, min_y, max_x, max_y):
        self.x = min_x + int(rnd.random() * (max_x - min_x))
        self.y = min_y + int(rnd.random() * (max_y - min_y))
