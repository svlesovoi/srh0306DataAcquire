#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>


#define dHlgrph_PkgDtMaxSz 4096
#define dHlgrph_PkgMaxSz (dHlgrph_PkgDtMaxSz + sizeof(tPkg_Head))
#define dHlgrph_PkgBlckDtMaxSz (dHlgrph_PkgDtMaxSz - sizeof(tDtBlock))
#define dHlgrph_PkgStrMaxSz (128 - sizeof(uint16_t))
#define dHlgrphSS_PkgMagic 0x05

/* Команды */
enum eRqst {
	/* Зарезервировано */
	eRqst_Reserved,
	/* Запросы общего назначения */
	eRqst_General_Begin = 0x01,
	/* Получить информацию о устройстве (формат данных JSON) */
	eRqst_GetProperty,
	/* Получить текущее состояние */
	eRqst_GetState,
	/* Установить текущее состояние */
	eRqst_SetStateProcessing,
	eRqst_SetStateIdle,
	eRqst_General_End,
	/* Команды X */ /* Зарезервированные команды */
	eRqst_X_Begin = 0x40,
	eRqst_X_End,
	/* Команды основного фунционала */
	eRqst_Rdr_Begin = 0x60,
	/* Установить конфигурацию */
	eRqst_Rdr_SetCnfg,
	/* Записать регистры */
	eRqst_Rdr_SetRgs32,
	eRqst_Rdr_End,
	/* Потоковые команды */
	/* Здесь определяем формат пересылаемых данных */
	eRqst_Rdr_Strm_Begin = 0xA0,
	eRqst_Rdr_Strm_DtOut,
	eRqst_Rdr_Strm_End,
	/* Команды прямого управления */
	eRqst_Rdr_CtrlDrct_Begin = 0xC0,
	eRqst_Rdr_CrtlDrct_End,
	eRqst_End
};

typedef struct sPkg_Head {
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
} tPkg_Head;

typedef struct sDtBlock {
    uint32_t Offset;
    uint32_t DtSz;
    uint32_t FullDtSz;
} tDtBlock;

typedef struct sSetRgstrs32Hd {
    uint32_t Count;
    uint32_t Rg[1];
} tSetRgstrs32Hd;

enum eState {
    eState_Idle, /* Состояние ожидания */
    eState_Processing, /* Состояние выполнения основной задачи */
    eState_End
};

/* Ошибки и сообщения */
typedef struct sError {
    uint16_t Code; /* Код ошибки */
    char Str[dHlgrph_PkgStrMaxSz]; /* Строка ошибки */
} tError;

typedef struct sState {
    uint16_t state; /* eState_Idle, eState_Processing */
    uint16_t numOfErros;
} tState;

typedef struct sConfigure {
 uint32_t Frequency;
 uint32_t Polarization;
 uint32_t Rsrvd0;
 uint32_t Rsrvd1;
} tConfigure;

typedef union uPkg_Dt {
 /* Не блочные данные */
    union {
        uint8_t pU8[dHlgrph_PkgDtMaxSz];
        tSetRgstrs32Hd Rg32;
        struct { tState Stt; tError Err; };
    };
 /* Блочные данные */
    struct {
        tDtBlock DtBlck;
        union {
            uint8_t pU8[dHlgrph_PkgBlckDtMaxSz];
            tSetRgstrs32Hd Rg32;
            tConfigure Cfg;
            struct { tState Stt; tError Err; };
        };
    } Blck;
} tPkg_Dt;

/* Структура пакета */
typedef union uPkg {
    uint8_t pU8[dHlgrph_PkgMaxSz];
    struct {
        tPkg_Head H; /* Заголовок */
        tPkg_Dt D;
    };
} tPkg;