from dot import Dot
from ship import Ship, Ships


N = 6  # размер поля
MAX_N = N ** 2  # макс. число ячеек


SYMBOLS = [
    [0, '.'],   # пустая ячейка
    [1, 'o'],   # пустая ячейка из окружения корабля
    [2, '*'],   # ячейка с промахом
    [3, '■'],   # ячейка с кораблём
    [4, '+'],   # ячейка с попаданием в корабль
    [5, 'X'],   # ячейка с потопленным кораблём
    ]

SHIP1 = 4   # количество 1-клеточных кораблей
SHIP2 = 2   # количество 2-клеточных кораблей
SHIP3 = 1   # количество 3-клеточных кораблей


class Board:
    def __init__(self):
        self.cells = [[0 for __ in range(N)] for _ in range(N)]
        self.ships_cells_count = SHIP1 + SHIP2 * 2 + SHIP3 * 3  # суммарное количество ячеек с кораблями
        self.owner = bool()     # True - пользователь, False - ПК
        self.hid = bool()       # скрывать корабли
        self._dot = Dot()       # текущая точка
        self._ships = Ships()   # список кораблей
        self.filled_cells = 0   # количество заполненных ячеек
        self.success_dot = Dot()  # текущая точка с успешным попаданием
        self.last_shot = False  # указатель, является ли выстрел повторным

    def field_zeroing(self):    # очистка поля
        self._ships.clear()
        self.cells = [[0 for __ in range(N)] for _ in range(N)]
        self.filled_cells = 0

    def dec_ships_cells_count(self):
        self.ships_cells_count -= 1

    def get_ships_cells_count(self):
        return self.ships_cells_count

    def get_cell(self, x, y):
        return self.cells[x][y]

    def set_cell(self, x, y, symbol_code):
        self.cells[x][y] = symbol_code

    def insert_ship(self, _ship):    # перенос координат кораблей в игровое поле
        for crd in _ship.coords:
            self.set_cell(crd.x, crd.y, SYMBOLS[3][0])
            self.filled_cells += 1

    def insert_contour(self, _ship):    # перенос координат контуров кораблей в игровое поле
        for _cntr in _ship.contour:
            if (self.cells[_cntr.x][_cntr.y] == 0):
                self.set_cell(_cntr.x, _cntr.y, SYMBOLS[1][0])
                self.filled_cells += 1

    def try_insert_ship(self, _start, _end, _len):
        for i in range(_start, _end):
            ship_error = False
            while not ship_error:     # повторяем попытки внести корабль
                if self.filled_cells == MAX_N:  # проверка на наличие свободных ячеек в поле
                    if self.owner:
                        print("Поле заполнено!")
                    return False
                self._ships._ship.append(Ship())
                self._ships._ship[i].create_ship(_len, self.owner, N)
                for crds in self._ships._ship[i].coords:
                    if (self.cells[crds.x][crds.y] == 1) or (self.cells[crds.x][crds.y] == 3):  # проверка - не попадают ли точки нового корабля на уже созданные корабли или их буферные зоны
                        ship_error = True  # попытка внесения корабля не удалась
                        if self.owner:
                            print("Расположение корабля в указанном месте невозможно, повторите ввод в другом месте.")
                        self._ships._ship.pop()     # удаляем последний корабль
                        break
                if ship_error:  # ошибка расположения корабля, повторяем попытки
                    ship_error = False
                    continue
                else:
                    break
            # попытка внесения корабля успешна:
            self.insert_ship(self._ships._ship[i])
            self._ships._ship[i].create_contour(N)
            self.insert_contour(self._ships._ship[i])
            if self.owner:
                print(f"Корабль №{i + 1} успешно добавлен.")
                self.print_board()
        return True

    def add_ships(self):
        self.try_insert_ship(0, SHIP3, 3)  # добавление 3-клеточного корабля
        self.try_insert_ship(SHIP3, SHIP3 + SHIP2, 2)   # добавление 2-клеточных кораблей
        if not self.try_insert_ship(SHIP3 + SHIP2, SHIP3 + SHIP2 + SHIP1, 1):   # добавление 1-клеточных кораблей
            return False    # значение для повторного заполнения поля
        return True

    def shot(self):
        while True:
            if not self.owner:
                self._dot.get_input(N, N)
            else:
                min_x, min_y = 0, 0
                max_x, max_y = N, N
                if self.last_shot:  # ограничиваем диапазон координат выстрела
                    if self.success_dot.x > 0:
                        min_x = self.success_dot.x - 1
                    if self.success_dot.y > 0:
                        min_y = self.success_dot.y - 1
                    if self.success_dot.x < N - 1:
                        max_x = self.success_dot.x + 2     # верхние границы не входят в диапазон
                    if self.success_dot.y < N - 1:
                        max_y = self.success_dot.y + 2     # верхние границы не входят в диапазон

                self._dot.get_random_dot(min_x, min_y, max_x, max_y)

            value = self.cells[self._dot.x][self._dot.y]
            if value == 2 or value == 4 or value == 5:  # попадание в уже отмеченную ячейку или в ячейку с подбитым/потопленным кораблём
                if not self.owner:  # игра на поле ПК
                    print("Указанная ячейка уже отмечена! Повторите ввод координат.")
                continue

            if self.owner:  # игра на поле пользоваеля
                print(f"Выстрел ПК: {self._dot.x + 1}, {self._dot.y + 1}")

            if value <= 1:     # попадание в пустую ячейку
                self.set_cell(self._dot.x, self._dot.y, 2)
                print("- промах.")
                self.last_shot = False
                return False     # повторный выстрел не нужен

            # осталась ситуация для кода "3" - попадание в корабль:
            self.set_cell(self._dot.x, self._dot.y, 4)  # отметка попадания в корабль

            for _ship in self._ships._ship:     # проверка на уничтожение корабля
                need_break = False
                for crds in _ship.coords:
                    if self._dot == crds:
                        _ship.hits[_ship.coords.index(crds)] = True
                        if _ship.length > 1:
                            print(f"- попадание в {_ship.length}-клеточный корабль!")
                            self.success_dot = self._dot    # сохранение текущей точки для поиска продолжений корабля
                            self.last_shot = True
                        need_break = True   # пометка для выхода из цикла перебора кораблей
                        break
                if _ship.ship_afloat and all(_ship.hits):   # если корабль отмечен как "на плаву", но все его ячейки подбиты
                    print(f"{_ship.length}-клеточный корабль полностью уничтожен!")
                    _ship.ship_afloat = False   # пометка корабля как потопленного
                    self.last_shot = False
                    for crds in _ship.coords:   # пометка координат корабля как полностью уничтоженного - код "5"
                        self.set_cell(crds.x, crds.y, 5)
                if need_break:  # выходим из цикла перебора кораблей - дальнейшая проверка не требуется
                    break

            self.dec_ships_cells_count()    # уменьшаем количество корабельных ячеек
            return True     # нужен повторный выстрел

    def print_board(self):  # печать текущего состояния игровго поля
        print("Поле", "игрока:" if self.owner else "ПК:")

        print(" \t", end='')
        for i in range(1, N + 1):  # вынесение номеров столбцов
            print(i, "\t", end='')
        for i in range(N):
            print()
            print(i + 1, "\t", end='')  # вынесение номеров строк
            for j in range(N):
                value = self.cells[j][i]
                if self.hid and (value == 1 or value == 3):  # если владелец текущей доски - ПК, ячейку с кораблём не показываем
                    value = 0
                print(SYMBOLS[value][1], "\t", end='')
        print()