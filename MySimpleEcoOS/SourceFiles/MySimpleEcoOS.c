/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   EcoVFB1
 * </сводка>
 *
 * <описание>
 *   Данный исходный файл является точкой входа
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */


/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IEcoTaskScheduler1.h"
#include "IdEcoTaskScheduler1Lab.h"
#include "IdEcoTimer1.h"
#include "IEcoSystemTimer1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "IdEcoIPCCMailbox1.h"
#include "IdEcoVFB1.h"
#include "IEcoVBIOS1Video.h"

/* Начало свободного участка памяти */
extern char_t __heap_start__;

/* Функция отладочного вывода (заглушка) */
void debug_print(const char* msg) {
    /* Заглушка для функции отладочного вывода */
}

/* Указатель на интерфейсы */
IEcoVFB1* g_pIVFB = 0;
IEcoVBIOS1Video* g_pIVideo = 0;
IEcoSystemTimer1* g_pISysTimer = 0;

/* Указатель на строку */
int32_t row_pointer = 0;

char_t g_strTask[2] = {0};

/* Для отображения прогресса выполнения задачи */
int32_t task_progress = 0; // в процентах

void TimerHandler(void) {
    /* g_pIMutex->pVTbl->Lock(g_pIMutex); */
    if (g_strTask[0] == '\\') {
        g_strTask[0] = '|';
    }
    else if (g_strTask[0] == '|') {
        g_strTask[0] = '/';
    }
    else if (g_strTask[0] == '/') {
        g_strTask[0] = '-';
    }
    else  {
        g_strTask[0] = '\\';
    }
    /* g_pIMutex->pVTbl->UnLock(g_pIMutex); */
}

void PrintDuration(uint64_t duration) {
	int32_t digit;
	size_t i;
	char* to_print = "Total duration: 0000000ms";
	for (i = 0; i < 7; ++i) {
		digit = duration % 10;
		to_print[22 - i] = '0' + digit;
		duration /= 10;
	}
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 26, 1, CHARACTER_ATTRIBUTE_FORE_COLOR_LIGHT_RED, to_print, 25);
}

void PrintResult(IEcoVBIOS1Video* pIVideo, int16_t result, uint32_t column, uint32_t row) {
	int32_t digit;
	char* to_print = "00";
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, column, row, CHARACTER_ATTRIBUTE_FORE_COLOR_LIGHT_RED, to_print, 2);
	if (result < 0) {
		to_print[0] = '-';
		result = -result;
	}
	digit = result % 10;
	to_print[1] = '0' + digit;
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, column, row, CHARACTER_ATTRIBUTE_FORE_COLOR_LIGHT_RED, to_print, 2);
}

void PrintPercent(int32_t percent, uint32_t column, uint32_t row) {
	int32_t digit;
	char* to_print = "00%";
	digit = percent % 10;
	to_print[1] = '0' + digit;
	percent /= 10;
	digit = percent % 10;
	to_print[0] = '0' + digit;
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, column, row, CHARACTER_ATTRIBUTE_FORE_COLOR_LIGHT_CYAN, to_print, 3);
}

void printProgress() {
	/* Выводим крутилку */
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 0, 1, CHARACTER_ATTRIBUTE_FORE_COLOR_LIGHT_CYAN, g_strTask, 1);
	/* Выводим пробел */
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 1, 1, CHARACTER_ATTRIBUTE_FORE_COLOR_LIGHT_CYAN, " ", 1);
	/* Выводим проценты */
	PrintPercent(task_progress, 2, 1);
}

void Task1(uint64_t duration) {
	PrintDuration(duration);
    uint64_t currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    uint64_t endTime = currentTime + duration;
    uint64_t changeTime = currentTime;
	uint64_t startTime = currentTime;
	task_progress = 0;
	++row_pointer;
    g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Job A initiated", 15);
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 12, 0, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Job A", 5);
    while ( endTime >= currentTime) {
        if (changeTime <= currentTime) {
			task_progress = (changeTime - startTime) * 100 / duration;
            printProgress();
            changeTime += duration / 100;
        }
        currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    }
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 25, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, " -> Job A completed", 19);
}

void Task2(uint64_t duration) {
	PrintDuration(duration);
    uint64_t currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    uint64_t endTime = currentTime + duration;
    uint64_t changeTime = currentTime;
	uint64_t startTime = currentTime;
	++row_pointer;
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Process B launched", 18);
    g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 12, 0, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Process B", 9);
	task_progress = 0;
    while ( endTime >= currentTime) {
        if (changeTime <= currentTime) {
			task_progress = (changeTime - startTime) * 100 / duration;
            printProgress();
            changeTime += duration / 100;
        }
        currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    }
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 25, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, " -> Process B terminated", 24);
}

void Task3(uint64_t duration) {
	PrintDuration(duration);
    uint64_t currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    uint64_t endTime = currentTime + duration;
    uint64_t changeTime = currentTime;
	uint64_t startTime = currentTime;
	task_progress = 0;
	++row_pointer;
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Operation C running", 19);
    g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 12, 0, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Operation C", 11);
    while ( endTime >= currentTime) {
        if (changeTime <= currentTime) {
			task_progress = (changeTime - startTime) * 100 / duration;
            printProgress();
            changeTime += duration / 100;
        }
        currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    }
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 25, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, " -> Operation C finished", 24);
}

void Task4(uint64_t duration) {
	PrintDuration(duration);
    uint64_t currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    uint64_t endTime = currentTime + duration;
    uint64_t changeTime = currentTime;
	uint64_t startTime = currentTime;
	task_progress = 0;
	++row_pointer;
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Activity D executing", 20);
    g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 12, 0, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Activity D", 10);
    while ( endTime >= currentTime) {
        if (changeTime <= currentTime) {
			task_progress = (changeTime - startTime) * 100 / duration;
            printProgress();
            changeTime += duration / 100;
        }
        currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    }
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 25, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, " -> Activity D done", 19);
}

void Task5(uint64_t duration) {
	PrintDuration(duration);
    uint64_t currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    uint64_t endTime = currentTime + duration;
    uint64_t changeTime = currentTime;
	uint64_t startTime = currentTime;
	task_progress = 0;
	++row_pointer;
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Work E executing", 16);
    g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 12, 0, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Work E", 6);
    while ( endTime >= currentTime) {
        if (changeTime <= currentTime) {
			task_progress = (changeTime - startTime) * 100 / duration;
            printProgress();
            changeTime += duration / 100;
        }
        currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    }
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 25, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, " -> Work E accomplished", 23);
}

void Task6(uint64_t duration) {
	PrintDuration(duration);
    uint64_t currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    uint64_t endTime = currentTime + duration;
    uint64_t changeTime = currentTime;
	uint64_t startTime = currentTime;
	task_progress = 0;
	++row_pointer;
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Procedure F started", 19);
    g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 12, 0, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Procedure F", 11);
    while ( endTime >= currentTime) {
        if (changeTime <= currentTime) {
			task_progress = (changeTime - startTime) * 100 / duration;
            printProgress();
            changeTime += duration / 100;
        }
        currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    }
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 25, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, " -> Procedure F ended", 21);
}

void Task7(uint64_t duration) {
	PrintDuration(duration);
    uint64_t currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    uint64_t endTime = currentTime + duration;
    uint64_t changeTime = currentTime;
	uint64_t startTime = currentTime;
	task_progress = 0;
	++row_pointer;
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Routine G activated", 19);
    g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 12, 0, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Routine G", 9);
    while ( endTime >= currentTime) {
        if (changeTime <= currentTime) {
			task_progress = (changeTime - startTime) * 100 / duration;
            printProgress();
            changeTime += duration / 100;
        }
        currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    }
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 25, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, " -> Routine G deactivated", 25);
}

void Task8(uint64_t duration) {
	PrintDuration(duration);
    uint64_t currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    uint64_t endTime = currentTime + duration;
    uint64_t changeTime = currentTime;
	uint64_t startTime = currentTime;
	task_progress = 0;
	++row_pointer;
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Execution H begun", 17);
    g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 12, 0, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Execution H", 11);
    while ( endTime >= currentTime) {
        if (changeTime <= currentTime) {
			task_progress = (changeTime - startTime) * 100 / duration;
            printProgress();
            changeTime += duration / 100;
        }
        currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    }
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 25, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, " -> Execution H concluded", 25);
}

void Task9(uint64_t duration) {
	PrintDuration(duration);
    uint64_t currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    uint64_t endTime = currentTime + duration;
    uint64_t changeTime = currentTime;
	uint64_t startTime = currentTime;
	task_progress = 0;
	++row_pointer;
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Function I commenced", 20);
    g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 12, 0, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Function I", 10);
    while ( endTime >= currentTime) {
        if (changeTime <= currentTime) {
			task_progress = (changeTime - startTime) * 100 / duration;
            printProgress();
            changeTime += duration / 100;
        }
        currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    }
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 25, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, " -> Function I terminated", 25);
}

void Task10(uint64_t duration) {
	PrintDuration(duration);
    uint64_t currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    uint64_t endTime = currentTime + duration;
    uint64_t changeTime = currentTime;
	uint64_t startTime = currentTime;
	task_progress = 0;
	++row_pointer;
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Module J processing", 19);
    g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 12, 0, CHARACTER_ATTRIBUTE_FORE_COLOR_MAGENTA, "Module J", 8);
    while ( endTime >= currentTime) {
        if (changeTime <= currentTime) {
			task_progress = (changeTime - startTime) * 100 / duration;
            printProgress();
            changeTime += duration / 100;
        }
        currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    }
	g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 25, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, " -> Module J complete", 21);
}

/*
 *
 * <сводка>
 *   Функция EcoMain
 * </сводка>
 *
 * <описание>
 *   Функция EcoMain - точка входа
 * </описание>
 *
 */
int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    /* Указатель на системный интерфейс */
    IEcoSystem1* pISys = 0;
    /* Указатель на интерфейс работы с системной интерфейсной шиной */
    IEcoInterfaceBus1* pIBus = 0;
    /* Указатель на интерфейс работы с памятью */
    IEcoMemoryAllocator1* pIMem = 0;
    IEcoMemoryManager1* pIMemMgr = 0;
    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    /* IEcoVirtualMemory1* pIVrtMem = 0; */  /* Недоступен */
    /* Указатель на интерфейс для работы с планировщиком */
    IEcoTaskScheduler1* pIScheduler = 0;
    IEcoTask1* pITask1 = 0;
    IEcoTask1* pITask2 = 0;
    IEcoTask1* pITask3 = 0;
	IEcoTask1* pITask4 = 0;
    IEcoTask1* pITask5 = 0;
    IEcoTask1* pITask6 = 0;
    IEcoTask1* pITask7 = 0;
    IEcoTask1* pITask8 = 0;
    IEcoTask1* pITask9 = 0;
    IEcoTask1* pITask10 = 0;
    /* Указатель на интерфейс для работы c буфером кадров видеоустройства */
    IEcoVFB1* pIVFB = 0;
    IEcoVBIOS1Video* pIVideo = 0;
    ECO_VFB_1_SCREEN_MODE xScreenMode = {0};
    /* Указатель на интерфейс для работы c системным таймером */
    IEcoSystemTimer1* pISysTimer = 0;
    /* Указатель на интерфейс для работы c таймером */
    IEcoTimer1* pITimer = 0;

/*
    IEcoCGI1* pIEcoCGI1 = 0;
    IEcoCGI1VirtualDevice* pIVD = 0;
    IEcoCGI1OneWayOutput* pIOutput = 0;
    ECO_CGI_1_VIEWPORT_POINT_t corner1 = {0};
    ECO_CGI_1_VIEWPORT_POINT_t corner2 = {0};
    ECO_CGI_1_POINT_t point1 = {0};
    ECO_CGI_1_POINT_t line[5] = { {10,10}, {100,100}, {50, 100}, {50,10}, {10,10}};
    ECO_CGI_1_POINT_LIST_t lineList = {5, line};
*/
    uint16_t offset = 0;
    uint16_t x1 = 7;
    uint16_t y1 = 45;
    uint16_t x2 = 139;
    byte_t color = 172; /* 3-3-2 bit RGB */

	/*uint64_t duration = 20ul;*/

    /* Создание экземпляра интерфейсной шины */
    result = GetIEcoComponentFactoryPtr_00000000000000000000000042757331->pVTbl->Alloc(GetIEcoComponentFactoryPtr_00000000000000000000000042757331, 0, 0, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    /* Проверка */
    if (result != 0 && pIBus == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с памятью */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoMemoryManager1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_0000000000000000000000004D656D31);
    //result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoMemoryManager1Lab, (IEcoUnknown*)GetIEcoComponentFactoryPtr_81589BFED0B84B1194524BEE623E1838);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с ящиком прошивки */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoIPCCMailbox1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_F10BC39A4F2143CF8A1E104650A2C302);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Запрос расширения интерфейсной шины */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        /* Установка расширения менаджера памяти */
        pIMemExt->pVTbl->set_Manager(pIMemExt, &CID_EcoMemoryManager1);
        //pIMemExt->pVTbl->set_Manager(pIMemExt, &CID_EcoMemoryManager1Lab);
        /* Установка разрешения расширения пула */
        pIMemExt->pVTbl->set_ExpandPool(pIMemExt, 1);
    }

    /* Получение интерфейса управления памятью */
    pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryManager1, (void**) &pIMemMgr);
    //pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1Lab, 0, &IID_IEcoMemoryManager1, (void**) &pIMemMgr);
    if (result != 0 || pIMemMgr == 0) {
        /* Возврат в случае ошибки */
        return result;
    }

    /* Выделение области памяти 512 КБ */
    pIMemMgr->pVTbl->Init(pIMemMgr, &__heap_start__, 0x080000);

    /* Получение интерфейса для работы с виртуальной памятью - отключено */
    /* result = pIMemMgr->pVTbl->QueryInterface(pIMemMgr, &IID_IEcoVirtualMemory1, (void**)&pIVrtMem);
    if (result == 0 && pIVrtMem != 0) {
        result = pIVrtMem->pVTbl->Init(pIVrtMem);
    } */
    /* Регистрация статического компонента для работы с планировщиком */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoTaskScheduler1Lab, (IEcoUnknown*)GetIEcoComponentFactoryPtr_902ABA722D34417BB714322CC761620F);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с таймером */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoTimer1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_8DB93F3DF5B947D4A67F7C100B569723);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с VBF */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoVFB1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_939B1DCDB6404F7D9C072291AF252188);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация мьютекса и семафора отключена */
    /* result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoMutex1Lab, (IEcoUnknown*)GetIEcoComponentFactoryPtr_2F48BBCBE4884CC08ECFC45990017215);
    if (result != 0) {
        goto Release;
    }

    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoSemaphore1Lab, (IEcoUnknown*)GetIEcoComponentFactoryPtr_0741985B8FD0476C867CAE177CD26E7C);
    if (result != 0) {
        goto Release;
    } */


    /* Получение интерфейса для работы с планировщиком */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoTaskScheduler1Lab, 0, &IID_IEcoTaskScheduler1, (void**) &pIScheduler);
    /* Проверка */
    if (result != 0 || pIScheduler == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }


	/* Получение интерфейса для работы с видео сервисами VBF */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoVFB1, 0, &IID_IEcoVFB1, (void**) &pIVFB);
    /* Проверка */
    if (result != 0 || pIVFB == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Получение информации о текущем режиме экрана */
    result = pIVFB->pVTbl->get_Mode(pIVFB, &xScreenMode);
    pIVFB->pVTbl->Create(pIVFB, 0, 0, xScreenMode.Width, xScreenMode.Height);

    /* Получение интерфейса IEcoVBIOS1Video для вывода текста */
    result = pIVFB->pVTbl->QueryInterface(pIVFB, &IID_IEcoVBIOS1Video, (void**) &pIVideo);
    if (result != 0 || pIVideo == 0) {
        goto Release;
    }

	    /* Получение мьютекса и семафора отключено */
    /* result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMutex1Lab, 0, &IID_IEcoMutex1, (void**) &g_pIMutex);
    if (result != 0 || g_pIMutex == 0) {
        goto Release;
    }

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoSemaphore1Lab, 0, &IID_IEcoSemaphore1, (void**) &g_pISemaphore);
    if (result != 0 || g_pISemaphore == 0) {
        goto Release;
    } */

    /* Получение интерфейса для работы с системным таймером */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoTimer1, 0, &IID_IEcoSystemTimer1, (void**) &pISysTimer);
    /* Проверка */
    if (result != 0 || pISysTimer == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
    g_pISysTimer = pISysTimer;

    /* Установка обработчика прерывания программируемого таймера */
    result = pISysTimer->pVTbl->QueryInterface(pISysTimer, &IID_IEcoTimer1, (void**)&pITimer);
    /* Проверка */
    if (result != 0 || pITimer == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

	pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 0, CHARACTER_ATTRIBUTE_FORE_COLOR_LIGHT_MAGENTA, "Status: ", 8);

    /* Инициализация */
    pIScheduler->pVTbl->InitWith(pIScheduler, pIBus, &__heap_start__+0x090000, 0x080000);

    /* Создание статических задач */
	row_pointer = 4;

    result = pIScheduler->pVTbl->NewTask(pIScheduler, Task1, 3500000ul, 0x100, &pITask1);
	pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_BLUE, "Job A registered", 16);
	++row_pointer;
    result = pIScheduler->pVTbl->NewTask(pIScheduler, Task2, 6000000ul, 0x100, &pITask2);
	pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_BLUE, "Process B registered", 20);
	++row_pointer;
    result = pIScheduler->pVTbl->NewTask(pIScheduler, Task3, 2500000ul, 0x100, &pITask3);
	pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_BLUE, "Operation C registered", 22);
	++row_pointer;
    result = pIScheduler->pVTbl->NewTask(pIScheduler, Task4, 9000000ul, 0x100, &pITask4);
	pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_BLUE, "Activity D registered", 21);
	++row_pointer;
    result = pIScheduler->pVTbl->NewTask(pIScheduler, Task5, 1200000ul, 0x100, &pITask5);
	pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_BLUE, "Work E registered", 17);
	++row_pointer;
    result = pIScheduler->pVTbl->NewTask(pIScheduler, Task6, 7500000ul, 0x100, &pITask6);
	pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_BLUE, "Procedure F registered", 22);
	++row_pointer;
    result = pIScheduler->pVTbl->NewTask(pIScheduler, Task7, 4000000ul, 0x100, &pITask7);
	pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_BLUE, "Routine G registered", 20);
	++row_pointer;
    result = pIScheduler->pVTbl->NewTask(pIScheduler, Task8, 10000000ul, 0x100, &pITask8);
	pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_BLUE, "Execution H registered", 22);
	++row_pointer;
    result = pIScheduler->pVTbl->NewTask(pIScheduler, Task9, 1800000ul, 0x100, &pITask9);
	pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_BLUE, "Function I registered", 21);
	++row_pointer;
    result = pIScheduler->pVTbl->NewTask(pIScheduler, Task10, 5200000ul, 0x100, &pITask10);
	pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 2, row_pointer, CHARACTER_ATTRIBUTE_FORE_COLOR_BLUE, "Module J registered", 19);
	if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
	row_pointer += 2;

    /* Вывод 1 строки "Эко ОС!!!" - кодовая страница 1251 */
    /* pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 1, 2, CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "Welcome to EcoOS!", 17); */

    /* Рисуем линию - подчеркивание */
    for (offset = x1; offset <= x2; offset++) {
        pIVideo->pVTbl->WriteDot(pIVideo, color, 0, offset, y1);
    }

	/* Запуск планировщика */
	pITimer->pVTbl->set_Interval(pITimer, 100000);
    pITimer->pVTbl->set_IrqHandler(pITimer, TimerHandler);
    pITimer->pVTbl->Start(pITimer);


    g_pIVFB = pIVFB;
    g_pIVideo = pIVideo;
    pIScheduler->pVTbl->Start(pIScheduler);

    while(1) {
        asm volatile ("NOP\n\t" ::: "memory");
    }

Release:

    /* Освобождение интерфейса для работы с интерфейсной шиной */
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }

    /* Освобождение интерфейса работы с памятью */
    if (pIMem != 0) {
        pIMem->pVTbl->Release(pIMem);
    }

    /* Освобождение интерфейса VFB */
    if (pIVFB != 0) {
        pIVFB->pVTbl->Release(pIVFB);
    }

    /* Освобождение системного интерфейса */
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }

    return result;
}

