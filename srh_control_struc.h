#ifndef SRH_CONTROL_STRUC_H
#define SRH_CONTROL_STRUC_H

#endif // SRH_CONTROL_STRUC_H

#define dHlgrph_PkgDtMaxSz 4096
#define dHlgrph_PkgMaxSz (dHlgrph_PkgDtMaxSz + sizeof(tPkg_Head))
//!!!!#define dHlgrph_PkgBlckDtMaxSz (dHlgrphSS_PkgDtMaxSz - sizeof(tDtBlock))
#define dHlgrph_PkgBlckDtMaxSz (dHlgrph_PkgDtMaxSz - sizeof(tDtBlock))
#define dHlgrph_PkgStrMaxSz (128 - sizeof(uint16_t))
#define dHlgrphSS_PkgMagic 0x05

#include <cstdint>

enum eRqst {
    eRqst_Reserved,                     // Зарезервировано
    eRqst_General_Begin = 0x01,         // Запросы общего назначения
    eRqst_GetProperty,                  // Получить информацию о устройстве (формат данных JSON)
    eRqst_GetState,                     // Получить текущее состояние
    eRqst_SetStateProcessing,           // Установить текущее состояние
    eRqst_SetStateIdle,
    eRqst_General_End,
    eRqst_X_Begin = 0x40,               // Команды X
    eRqst_X_End,
    eRqst_Rdr_Begin = 0x60,             // Команды основного фунционала
    eRqst_Rdr_SetCnfg,                  // Установить конфигурацию
    eRqst_Rdr_SetRgs32,                 // Записать регистры
    eRqst_Rdr_End,
    eRqst_Rdr_Strm_Begin = 0xA0,        // Здесь определяем формат пересылаемых данных
    eRqst_Rdr_Strm_DtOut,
    eRqst_Rdr_Strm_End,
    eRqst_Rdr_CtrlDrct_Begin = 0xC0,    // Команды прямого управления
    eRqst_Rdr_CrtlDrct_End,
    eRqst_End
};

enum eState {
    eState_Idle,                        // Состояние ожидания
    eState_Processing,                  // Состояние выполнения основной задачи
    eState_End
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
            uint32_t polarization:1;
            uint32_t frequencyIndex:8;
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

typedef struct sError {
    uint16_t Code;                  // Ошибки и сообщения
    char Str[dHlgrph_PkgStrMaxSz];
} tError;

typedef struct sState {
    uint16_t state;                 // eState_Idle, eState_Processing
    uint16_t numOfErros;
} tState;

typedef union uPkg_Dt {
        union {                                         // Не блочные данные
            uint8_t pU8[dHlgrph_PkgDtMaxSz];
            tSetRgstrs32Hd Rg32;
            struct {
                tState Stt;
                tError Err;
            };
        };
        struct {                                        // Блочные данные
            tDtBlock DtBlck;
            union {
                uint8_t pU8[dHlgrph_PkgBlckDtMaxSz];
                tSetRgstrs32Hd Rg32;
                struct {
                    tState Stt;
                    tError Err;
                };
            };
        } Blck;
} tPkg_Dt;

typedef union uPkg {                                    // Структура пакета
    uint8_t pU8[dHlgrph_PkgMaxSz];
    struct {
        tPkg_Head H;                                    // Заголовок
        tPkg_Dt D;
    };
} tPkg;
