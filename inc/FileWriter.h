#pragma once

#include <IStreamWriter.h>
#include <ThreadPool.h>


/**
 * @brief Класс вывода блока команд в файл.
 */
class FileWriter : public IStreamWriter, public ThreadPool<2> {
  public:
    explicit FileWriter() {}

    ~FileWriter() override {
      stop();
    }

    /**
     * @brief Запись блока команд в поток.
     * @param context_id - id контекста из которого производится вывод.
     * @param bulk - блок команд.
     */
    void write(uint8_t context_id, const Bulk& bulk) final;

};

