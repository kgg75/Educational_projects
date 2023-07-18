import random as rnd
from dot import Dot


class Ship:
    def __init__(self):
        self.length = 1         # длина корабля
        self.orient = True      # ориентация корабля: True - горизонтальный, False - вертикальный
        self.coords = []        # списак объектов класса Dot()
        self.contour = []       # списак объектов класса Dot()
        self.hits = []          # список для фиксации попаданий
        self.ship_afloat = True # корабль на плаву

    def create_contour(self, _N):
        dot8 = [Dot() for _ in range(8)]    # список окружающих точек
        for _dot in self.coords:
            dot8[0] = Dot(_dot.x - 1, _dot.y - 1)
            dot8[1] = Dot(_dot.x, _dot.y - 1)
            dot8[2] = Dot(_dot.x + 1, _dot.y - 1)
            dot8[3] = Dot(_dot.x - 1, _dot.y)
            dot8[4] = Dot(_dot.x + 1, _dot.y)
            dot8[5] = Dot(_dot.x - 1, _dot.y + 1)
            dot8[6] = Dot(_dot.x, _dot.y + 1)
            dot8[7] = Dot(_dot.x + 1, _dot.y + 1)

            for each_dot in dot8:
                if (each_dot.x >= 0) and (each_dot.y >= 0) and (each_dot.x < _N) and (each_dot.y < _N):   # проверка, не выходит ли точка контура за игровое поле
                    if (self.length == 1) or ((not each_dot in self.coords) and (not each_dot in self.contour)):    # проверка, не попадает ли точка конутра в корабль или уже имеющиеся точки контра
                        self.contour.append(each_dot)

    def get_ship_orient(self):
        while True:
            try:
                value = bool(int(input("Введите расположение корабля (1 - горизонтальное, 0 - вертикальное): ")))
            except ValueError:
                print("Ошибка ввода!")
            else:
                return value

    def create_ship(self, _len, _owner, _N):
        _dot = Dot()
        self.length = _len

        if _owner:  # сообщение нужно только пользователю
            print(f"Добавление {_len}-клеточного корабля -")

        # выбор ориенации корабля: True - горизонтальный, False - вертикальный
        if _len > 1:
            if _owner:
                self.orient = self.get_ship_orient()
            else:
                self.orient = bool(rnd.random() < 0.5)

        dx = _N - int(self.orient) * (_len - 1)     # - ограничитель поля по горизонтали
        dy = _N - int(not self.orient) * (_len - 1)     # - ограничитель поля по вертикали

        if _owner:
            if _len > 1:
                print("Укажите", ("левую" if self.orient else "верхнюю"), "точку -")
            _dot.get_input(dx, dy)
        else:
            _dot.get_random_dot(0, 0, dx, dy)

        self.coords.append(_dot)
        self.hits.append(False)

        dx = int(self.orient)   # расчёт приращения по X
        dy = int(not self.orient)   # расчёт приращения по Y

        for j in range(1, self.length):
            self.coords.append(Dot(self.coords[j - 1].x + dx, self.coords[j - 1].y + dy))
            self.hits.append(False)

class Ships:
    def __init__(self):
        self._ship = []     # список кораблей

    def append(self, _ship):
        self._ship.append(_ship)

    def pop(self, index):
        self._ship.pop(index)

    def clear(self):
        self._ship.clear()
