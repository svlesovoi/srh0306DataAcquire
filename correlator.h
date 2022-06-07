/******************************************************************************/
/*    Filename: hlgrph_prtcl_ext.h
 *   Developer: Rusanovskij Andrej Petrovich
 * Description: Команды сервера SpeedyM */
/******************************************************************************/

/*============================================================================*/
#ifndef hlgrph_prtcl_ext_H
#define hlgrph_prtcl_ext_H

/*----------------------------------------------------------------------------*/
#include "stdint.h"
#include <unistd.h>
#include <sys/types.h>

/*============================================================================*/
//namespace nHlgrph {

/*----------------------------------------------------------------------------*/
/* Максимальный размер данных пакета */
#define dHlgrph_PkgDtMaxSz 4095
/* Максимальный размер пакета */
#define dHlgrph_PkgMaxSz (dHlgrph_PkgDtMaxSz + sizeof(tPkg_Head))
/* Максимальный рамер передаваемой строки */
#define dHlgrph_PkgStrMaxSz (128 - sizeof(uint16_t))
/* максимальный размер данных блочного пакета */
#define dHlgrph_PkgBlckDtMaxSz (dHlgrph_PkgDtMaxSz - sizeof(tDtBlock))
/* Код заголовка пакетов */
#define dHlgrph_PkgMagic 0x05

#define dHlgrphSS_PkgDtMaxSz 4095
#define dHlgrphSS_PkgMaxSz (dHlgrphSS_PkgDtMaxSz + sizeof(tPkg_Head))
#define dHlgrphSS_PkgBlckDtMaxSz (dHlgrphSS_PkgDtMaxSz - sizeof(tDtBlock))
#define dHlgrphSS_PkgMagic 0x05
#define dHlgrph_PkgStrMaxSz (128 - sizeof(uint16_t))

//#define dPrtkl_3_0
/* размеры для "мета" пакета */
#define dSzMetaDt  32
#define dSzRsrvd1  (dSzMetaDt / 2 - 7)
#define dSzRsrvd2  (dSzMetaDt / 2 - sizeof(tPkgSPI)/sizeof(uint32_t))

/* Данные для прикладного уровня */
typedef struct sPkgSPI {
    union {
      volatile uint U32;              /* Читаем "5"(то spi работает) */
      struct {
       volatile uint Addr:30;         /* Адрес регистра */
       volatile uint Rsrv:1;          /* Выкл./Вкл инкримент */
       volatile uint R_W:1;           /* Чтение/Запись в регистр */
      };
    } Cntrl;
    union {
        volatile uint U32;
        struct {
            volatile uint Ready:1;    /* Отсутствие "clk" */
            volatile uint Error:1;    /* Ошибки */
            volatile uint Valid:1;    /* Пока не используется  */
            volatile uint Plrztn:1;   /* Информация по поляризации */
//            volatile uint Rsrvd:28;   /* Резерв */
            volatile uint Rsrvd:12;   /* Резерв */
            volatile uint Tbox:8;   /* Резерв */
            volatile uint Tfpga:8;   /* Резерв */
        };
    } StsSpi;
    union {
      volatile uint U32;           /*  Состояние   */
      struct {
          volatile uint Link:16;    /* "Link" состояние связи с приёмником  */
          volatile uint Rsrvd:16;
      };
    } State;
    volatile uint Mode;           /* Режим   */
    volatile uint TimePrescaler;  /* Предделитель времени  */
    volatile uint Time;           /* Время   */
    volatile uint DataDelay;      /* Задрежка данных   */
    volatile uint DataDuration;   /* Продолжительность данных   */
    volatile uint FrequencyDelay; /* Задержка частоты     */
    volatile uint Frequency;      /* Частота     */
} tPkgSPI;


//#define dPrtkl_3_0   /* Чтобы была возможность быстро откатиться назад  */
/*----------------------------------------------------------------------------*/
/* Команды */
enum eRqst {
  /* Зарезервировано */
  eRqst_Reserved,

  /* Запросы общего назначения */ /* Базовые команды одинаковые для всех устройств */
  eRqst_General_Begin = 0x01,
  /* Получить информацию о устройстве (формат данных JSON) */
  eRqst_GetProperty,
  /* Получить текущее состояние */
  eRqst_GetState,
  /* Установить текущее состояние */
  eRqst_SetStateProcessing,
  eRqst_SetStateIdle,
  eRqst_General_End,

  /* Команды X */                     /* Зарезервированные команды */
  eRqst_X_Begin = 0x40,
  eRqst_SetEmulSin,
  eRqst_GetPropertyFPGA,
  eRqst_X_End,

  /* Команды основного фунционала */  /* Специфические команды для данного конкретного устройства */
  eRqst_Rdr_Begin = 0x60,
  /* Установить конфигурацию */
  eRqst_Rdr_SetCnfg,                  /*  Убрать не используется  */
  /* Записать регистры */
  eRqst_Rdr_SetRgs32,
  /* Установить время в FPGA */
  eRqst_SetTime,
  eRqst_Rdr_End,

  /* Потоковые команды */            /* Здесь определяем формат пересылаемых данных(fft,log,src)     */
  eRqst_Rdr_Strm_Begin = 0xA0,
  eRqst_Rdr_Strm_DtOut,
  eRqst_Rdr_Strm_End,

  /* Команды прямого управления */  /* Управление конретными устройствами(ДД,ППМ и т.д.) */
  eRqst_Rdr_CtrlDrct_Begin = 0xC0,
  eRqst_Rdr_CrtlDrct_End,

  eRqst_End
};

enum eSyncDriverRqst {
    /* Зарезервировано */
    eSyncDriverRqst_Reserved,

    /* Запросы общего назначения */
    eSyncDriverRqst_General_Begin = 0x01,

    /* Получить информацию о устройстве (формат данных JSON) */
    eSyncDriverRqst_GetProperty,

    /* Получить текущее состояние */
    eSyncDriverRqst_GetState,

    /* Установить текущее состояние */
    eSyncDriverRqst_SetStateProcessing,
    eSyncDriverRqst_SetStateIdle,
    eSyncDriverRqst_General_End,

    /* Команды X */
    eSyncDriverRqst_X_Begin = 0x40,
    eSyncDriverRqst_X_End,

    /* Команды основного фунционала */
    eSyncDriverRqst_Rdr_Begin = 0x60,

    /* Установить конфигурацию */
    eSyncDriverRqst_Rdr_SetCnfg,

    /* Получить конфигурацию */
    eSyncDriverRqst_Rdr_GetCnfg,
    eSyncDriverRqst_Rdr_SetCnfgNew,
    eSyncDriverRqst_Rdr_GetCnfgNew,
    eSyncDriverRqst_Rdr_End,

    /* Потоковые команды */
    eSyncDriverRqst_Rdr_Strm_Begin = 0xA0,
    eSyncDriverRqst_Rdr_Strm_End,

    /* Команды прямого управления */
    eSyncDriverRqst_Rdr_CtrlDrct_Begin = 0xC0,
    eSyncDriverRqst_Rdr_CrtlDrct_End,
    eSyncDriverRqst_End
};

/* Состояние */
enum eState {
  eState_Idle,        /* Состояние ожидания */
  eState_Processing,  /* Состояние выполнения основной задачи */
  eState_End
};

/* Результат выполнения запроса */
enum eRqstRtrn {
  eRqstR_Ok,    /* Запрос выполнен успешно */
  eRqstR_Fault, /* Запрос не выполнен (see код ошибки) */
  eRqstR_End
};

/* Определение состояния (вкл. - выкл.) */
enum eSts {
  eSts_Off, /* Выкл. */
  eSts_On,  /* Вкл. */
  eSts_End
};

/* Тип данных */
enum eTypeOut {
  eTpOut_Raw,             /* Данные FPGA без обработки */
  eTpOut_End
};

/* Раширения заголовка */
enum ePkg_HeadExtTp {
  ePkg_HeadExtTp0, /* Расширения нет */
  ePkg_HeadExtTp1, /* Первое расширение */
  ePkg_HeadExtTp_End
};

/*============================================================================*/
#pragma pack(push, 1)
/*============================================================================*/
/* Заголовок пакета */
/*----------------------------------------------------------------------------*/
/* Описание полей заголовка:
 *
 * Функциональный заголовок:
 * RqstHeadTpExt - Определяет расширение заголовка пакета
 * RqstHeadTpExt = 0 - Расширение заголовка отсутсвует
 * RqstHeadTpExt = N - Раширение заголовка sPkg_HeadAddonN (N < 0x0F)
 *
 * Magic - Постоянное число = 0x05
 *
 * Rqst - Код запроса [0..0xFF]
 *
 * DtSz - Размер данных (max = 4095 = 0x0FFF)
 * isRqstRdy - Статус выполнения запроса (устанавливается при ответе)
 * isPacked - Режим блочной передачи данных
 * isCrypt - Режим шифрования данных */

typedef struct sPkg_Head {
  union {
    uint32_t All_U32;       /* Полный заголовок пакета */
    uint8_t  pU8[sizeof(uint32_t)];

    struct {
      uint8_t  Magic_U8;    /* Функциональный заголовок */
      uint8_t  Rqst_U8;     /* Код запроса */
      uint16_t Prmtr_U16;   /* Параметры запроса */
    };

    struct {
      /* [0..7]:8 - Функциональный заголовок */
      uint32_t HeadExtTp:4; /* Тип заголовка пакета [0x00..0x0F] */
      uint32_t Magic:4;     /* Magic код = 0x05 */
      /* [0..7]:8 - Код запроса */
      uint32_t Rqst:8;      /* Код запроса [0x00..0xFF] */
      /* [16..31]:16 - Параметры запроса */
      uint32_t DtSz:12;     /* Размер данных пакета (max = 4095 = 0x0FFF) */
      uint32_t isRqstR:1;   /* Результат выполнения запроса (0 - Ok, 1 - Fault) */
      uint32_t isPacked:1;  /* Пакетные данные (0 - Off, 1 - On) */
      uint32_t isCrypt:1;   /* Пакетные данные зашифрованы (0 - Off, 1 - On) */
      uint32_t Rsrvd:1;     /* Резерв = NA */
    };
  };
} tPkg_Head;

/*----------------------------------------------------------------------------*/
/* Расширения 1 заголовка */
typedef struct sPkg_HeadExtTp1 {
  /* Идентификатор клиента (MAC, 0 - отсутсвует, (-1) - все) */
  uint64_t IDClient;
  /* Идентификатор сервера (MAC, 0 - отсутсвует, (-1) - все) */
  uint64_t IDServer;
  /* Идентификационный номер пакета (с каждым запросом инкрементировать на 1) */
  uint32_t IDPkg;
} tPkg_HeadExtTp1;


/*============================================================================*/
/* Структуированные данные пакета */
/*----------------------------------------------------------------------------*/
/* Описатель блока данных (для передачи большого блока маленькими блоками)
 * Если DtSz = Offset - окончание передачи данных
 * Если Offset = 0 - начало передачи данных */
typedef struct sDtBlock {
  uint32_t Offset;   /* Смещение блока от начала данных */
  uint32_t DtSz;     /* Размер передаваемого блока */
  uint32_t FullDtSz; /* Размер передаваемых данных (полный размер данных) */
} tDtBlock;

/*----------------------------------------------------------------------------*/
/* Ошибки и сообщения */
typedef struct sError {
  uint16_t Code;                   /* Код ошибки */
  char Str[dHlgrph_PkgStrMaxSz];  /* Строка ошибки */
} tError;

typedef struct sState {
  uint16_t state;         /* eState_Idle, eState_Processing */
  uint16_t numOfErros;
} tState;

#ifdef dPrtkl_3_0
/*----------------------------------------------------------------------------*/
typedef struct sConfigure {
 uint32_t Frequency;
 uint32_t Polarization;
 uint32_t Rsrvd0;
 uint32_t Rsrvd1;
} tConfigure;
#else
/*----------------------------------------------------------------------------*/
typedef struct sConfigure {
    union {
        uint32_t DtArr[dSzMetaDt];
        struct {
            uint32_t CountPkg;       /* Счетчик пакетов(после запуска сервера) */
            uint32_t CountDMA_IRQ;   /* Счетчик прерываний */
            uint32_t CountDMAOvrld;  /* Перегрузки буфера */
            uint32_t CountFifoOvrld; /* Счётчик перегрузок Fifo */
            uint64_t TimeDMA;        /* Время приёма пакета корреляции */
            uint32_t Temprtr;        /* Температура среды */
            uint32_t RsrvdArr1[dSzRsrvd1];
            tPkgSPI  InfoSpiDrv;
            uint32_t RsrvdArr2[dSzRsrvd2];
        };
    };
} tConfigure;
#endif

typedef struct sPropertyFPGA {
    uint32_t Id;           /* Индификатор прошивки FPGA */
    uint32_t TimeStamp;    /* Временная метка    */
    uint32_t SzFifo;       /* Размер SPI Fifo буфера */
    uint32_t IdHdl;        /* Индификатор прошивки HDL*/
    uint32_t TimeStampHdl; /* Временная метка  HDL    */
} tsPropertyFPGA;

/*----------------------------------------------------------------------------*/
typedef struct tSetRgstrs32Hd {
  uint32_t Count;  /* Размер данных */
  uint32_t Rg[1];  /* Данные регистра */
} tSetRgstrs32Hd;

/*============================================================================*/
typedef union uPkg_Dt {
  /* Не блочные данные */
  union {
    uint8_t pU8[dHlgrph_PkgDtMaxSz];
    tConfigure Cfg;
    tSetRgstrs32Hd Rg32;
    tsPropertyFPGA PropertyFPGA;
    struct { tState Stt; tError Err; };
  };

  /* Блочные данные */
  struct {
    tDtBlock DtBlck;
    union {
      uint8_t pU8[dHlgrph_PkgBlckDtMaxSz];
      tConfigure Cfg;
      tSetRgstrs32Hd Rg32;
      tsPropertyFPGA PropertyFPGA;
      struct { tState Stt; tError Err; };
    };
  } Blck;

} tPkg_Dt;


/*============================================================================*/
/* Общая структура пакета */
/*----------------------------------------------------------------------------*/
typedef union uPkg {
  uint8_t pU8[dHlgrph_PkgMaxSz];

  struct {
    tPkg_Head H;  /* Заголовок */
    tPkg_Dt D;
  };

} tPkg;

/*----------------------------------------------------------------------------*/
typedef union uPkg_Tp1 {
  uint8_t pU8[dHlgrph_PkgMaxSz];

  struct {
    tPkg_Head H;        /* Заголовок */
    tPkg_HeadExtTp1 H1; /* Расширение загоовка */
    tPkg_Dt D;
  };

} tPkg_Tp1;


//-----------------------------------------------------------------------------------------
typedef struct SyncDriverPkg_Head {
    union {
        uint32_t All_U32;
        uint8_t pU8[sizeof(uint32_t)];
        struct {
            uint8_t Magic_U8;
            uint8_t Rqst_U8;
            uint16_t Prmtr_U16;
        };
        struct {
            uint32_t HeadExtTp:4;
            uint32_t Magic:4;
            uint32_t Rqst:8;
            uint32_t DtSz:12;
            uint32_t isRqstR:1;
            uint32_t isPacked:1;
            uint32_t isCrypt:1;
            uint32_t Rsrvd:1;
        };
    };
} tSyncDriverPkg_Head;

typedef struct SyncDriverDtBlock {
    uint32_t Offset;
    uint32_t DtSz;
    uint32_t FullDtSz;
} tSyncDriverDtBlock;

typedef struct SyncDriverConfigure {
    uint32_t FSetDuration;
    uint32_t PolarToggle;
    uint32_t PolarSet;
    uint32_t HiTimeDuration;
    uint32_t LowTimeDuration;
    uint32_t NBand;
    uint32_t NCycle;
    uint32_t NFSet;
    uint32_t Cntrl;
} tSyncDriverConfigure;

typedef enum SyncDriverFrqRange {
    SyncDriverFrqRange_3_6,
    SyncDriverFrqRange_6_12,
    SyncDriverFrqRange_12_24,
    SyncDriverFrqRange_End
} tSyncDriverFrqRange;

typedef struct SyncDriverConfigureNew {
   tSyncDriverFrqRange FrqRange;
   tSyncDriverConfigure Configure;
} tSyncDriverConfigureNew;

typedef union uSyncDriverPkg_Dt {
    /* Не блочные данные */
    union {
        uint8_t pU8[dHlgrphSS_PkgDtMaxSz];
        tSyncDriverConfigure Cfg;
        tSyncDriverConfigureNew CfgNew;
        tSyncDriverFrqRange FrqRange;
        struct { tState Stt; tError Err; };
    };
    /* Блочные данные */
    struct {
        tSyncDriverDtBlock DtBlck;
        union {
            uint8_t pU8[dHlgrphSS_PkgBlckDtMaxSz];
            tSyncDriverConfigure Cfg;
            tSyncDriverConfigureNew CfgNew;
            tSyncDriverFrqRange FrqRange;
        };
    } Blck;
} tSyncDriverPkg_Dt;

typedef union uSyncDriverPkg {
    uint8_t pU8[dHlgrphSS_PkgMaxSz];
    struct {
        tSyncDriverPkg_Head H;
        tSyncDriverPkg_Dt D;
    };
} tSyncDriverPkg;

//-----------------------------------------------------------------------------------------

/*============================================================================*/
#pragma pack(pop)
/*============================================================================*/

/* Ошибки выдаются синхронным и асинхронным методами.
 * Синхронный - это вариант выдачи ошибки на установленный запрос, через
 * интерфейс связи.
 * Асинхронный - это вариант самостоятельной отправки ошибки чрез интерфейс связи.
 * Ошибки передаются при возникновении ошибки в рабочих модулях программы. */

/* Код ошибки */
typedef int tErrCd; /* see enum eErrorCode (last nHlgrph::eErrCd_End) */
enum eErrorCode {
  /* Общие ошибки */
  eErrCd_NDef = 1,    /* Ошибка не определена */
  eErrCd_Fatal, /* Фатальная ошибка (продолжение работы программы не возможно) */
  eErrCd_OutRange,    /* Размер данных должен быть меньше */
  eErrCd_FlNotExist,  /* Файл не существует */
  eErrCd_FlNotOpen,   /* Файл не открыт */
  eErrCd_FlCantOpen,  /* Невозможно открыть файл */
  eErrCd_FlCantRd,    /* Невозможно прочитать файл */
  eErrCd_FlCantWr,    /* Невозможно записать файл */
  eErrCd_CantFillPrprty, /* Невозможно заполнить информацию */

  eErrCd_SPI_RdWr,    /* Ошибка работы по SPI */

  /* Ошибки запросов - запрос не выполнен */
  eErrCd_Rqst_NSpprt,   /* Запрос не поддерживается */
  eErrCd_Rqst_BadPrmtr, /* Неверные параметр запроса */

  eErrCd_End
};

#define dHlgrph_ErrCdStrEn /* max len string = dHlgrph_PkgStrMax_Sz = 126 byte*/ \
 ""\
 ,"Not define error"                /*eErrCd_NDef*/\
 ,"Fatal (sytem will be restart)"   /*eErrCd_Ftl*/\
 ,"Overload data cache"             /*eErrCd_OutRange*/\
 ,"File not exist"                  /*eErrCd_FlNotExist*/\
 ,"File not open"                   /*eErrCd_FlNotOpen*/\
 ,"Cant open file"                  /*eErrCd_CantOpenFl*/\
 ,"Cant read file"                  /*eErrCd_CantRdFl*/\
 ,"Cant write file"                 /*eErrCd_CantWrFl*/\
 ,"Cant fill property"              /*eErrCd_CantFillPrprty*/\
 \
 ,"SPI read write"                  /*eErrCd_SPI_RdWr*/\
 \
 ,"Request not support"             /*eErrCd_Rqst_NSpprt*/\
 ,"Request bad parametr"            /*eErrCd_Rqst_BadPrmtr*/\

#define dHlgrph_ErrCdStrRu /* max len string = dHlgrph_PkgStrMax_Sz = 126 byte*/ \
 ""\
 ,"Неопределенная ошибка"           /*eErrCd_NDef*/\
 ,"Фатальная ошибка"                /*eErrCd_Ftl*/\
 ,"Перегрузка области памяти"       /*eErrCd_OutRange*/\
 ,"Файл не существует"              /*eErrCd_FlNotExist*/\
 ,"Файл не открыт"                  /*eErrCd_FlNotOpen*/\
 ,"Невозможно открыть файл"         /*eErrCd_CantOpenFl*/\
 ,"Невозможно прочитать файл"       /*eErrCd_CantRdFl*/\
 ,"Невозможно записать в файл"      /*eErrCd_CantWrFl*/\
 ,"Невозможно определить свойсва программы" /*eErrCd_CantFillPrprty*/\
 \
 ,"Ошибка работы по SPI"            /*eErrCd_SPI_RdWr*/\
 \
 ,"Запрос не поддерживается"        /*eErrCd_Rqst_NSpprt*/\
 ,"Недопустимый параметр запроса"   /*eErrCd_Rqst_BadPrmtr*/\

/*----------------------------------------------------------------------------*/
//} /* End namespace */


/*============================================================================*/
/* Changes in source files:
 * 170705 - Первый релиз */
/*----------------------------------------------------------------------------*/
#endif
