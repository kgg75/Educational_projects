from board import Board


GAMER_NAME = ["ПК", "пользователь"]


class Game:
    boards = [Board(), Board()]
    gamer = True  # False - ПК, True - пользователь

    @staticmethod
    def greet():
        print("Начинаем игру 'Морской бой'!\n"
              "Ввод ходов производится через пробел или запятую.\n"
              "Первое значение - координата по горизонтали, второе - по вертикали.")

    def start(self):
        self.greet()

        self.boards[0].owner = False    # поле ПК
        self.boards[1].owner = True     # поле игрока

        self.boards[0].hid = True       # скрывать корабли на поле ПК
        self.boards[1].hid = False      # не скрывать корабли на поле игрока

        print("Добавление кораблей для ПК -")
        while not self.boards[0].add_ships():
            self.boards[0].field_zeroing()  # обнуляем поле и заполняем заново
        print(" - выполнено.")

        print("Добавление кораблей для пользователя -")
        while not self.boards[1].add_ships():
            print("Заполнение поля в данной конфигурации невозможно! Начните заполнять заново.")
            self.boards[1].field_zeroing()

        print("-" * 40,"\nНачинаем игру!")
        self.boards[0].print_board()
        self.boards[1].print_board()

    def loop(self):
        while True:
            print(f"Кто играет: {GAMER_NAME[int(self.gamer)]}")
            while self.boards[int(not self.gamer)].shot():  # выстрел по доске соперника; если удачно - повторный
                self.boards[int(not self.gamer)].print_board()  # печать доски соперника
                if self.boards[int(not self.gamer)].get_ships_cells_count() == 0:     # все корабли уничтожены
                    print(f"Все корабли уничтожены!\nПобедил {GAMER_NAME[int(self.gamer)]}.")
                    return
                print("Повторный выстрел -")

            self.boards[int(not self.gamer)].print_board()  # печать доски соперника
            self.gamer = not self.gamer
