#pragma once

#include <Bulk.h>

#include <ctime>
#include <string>
#include <vector>

/**
 * @brief Интерфейс для классов выводящих блок команд в поток.
 */
class IStreamWriter {
  public:
    virtual ~IStreamWriter() = default;

    /**
     * @brief Запись блока команд в поток.
     * @param context_id - id контекста из которого производится вывод.
     * @param bulk - блок команд.
     */
    virtual void write(uint8_t context_id, const Bulk& bulk) = 0;
};

