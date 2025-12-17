# Инструкция по сборке и запуску

### Шаг 1: Установка переменных окружения

```bash
export ECO_FRAMEWORK=/home/artem-kholev/Desktop/Dev
export ECO_TOOLCHAIN=/home/artem-kholev/Desktop/Eco.Lab4
```

### Шаг 2: Сборка библиотеки планировщика задач

```bash
cd Eco.TaskScheduler1Lab/AssemblyFiles/EcoOS/aarch64_gcc_13_2_1/
make clean
make TARGET=1 DEBUG=0
```

### Шаг 3: Сборка основного проекта

```bash
cd ../../../../MySimpleEcoOS/AssemblyFiles/EcoOS/aarch64_gcc_13_2_1/
make clean
make Pi3-64
```

После успешной сборки в текущей директории появится файл `kernel8.img`.

### Шаг 4: Запуск в QEMU

```bash
cd /home/artem-kholev/Desktop/Eco.Lab4
env -i PATH=/usr/bin:/bin TERM=$TERM HOME=$HOME DISPLAY=$DISPLAY XAUTHORITY=$XAUTHORITY /usr/bin/qemu-system-aarch64 -M raspi3b -kernel MySimpleEcoOS/AssemblyFiles/EcoOS/aarch64_gcc_13_2_1/kernel8.img
```


### Задачи в проекте

В проекте зарегистрировано 10 задач с разными значениями duration:
1. **Task1** (Job A) - 3.5 сек
2. **Task2** (Process B) - 6.0 сек
3. **Task3** (Operation C) - 2.5 сек
4. **Task4** (Activity D) - 9.0 сек
5. **Task5** (Work E) - 1.2 сек
6. **Task6** (Procedure F) - 7.5 сек
7. **Task7** (Routine G) - 4.0 сек
8. **Task8** (Execution H) - 10.0 сек (наивысший приоритет)
9. **Task9** (Function I) - 1.8 сек
10. **Task10** (Module J) - 5.2 сек

Порядок выполнения (от наибольшего приоритета):
1. Execution H (10.0 сек)
2. Activity D (9.0 сек)
3. Procedure F (7.5 сек)
4. Process B (6.0 сек)
5. Module J (5.2 сек)
6. Routine G (4.0 сек)
7. Job A (3.5 сек)
8. Operation C (2.5 сек)
9. Function I (1.8 сек)
10. Work E (1.2 сек)

