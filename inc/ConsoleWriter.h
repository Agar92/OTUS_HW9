#pragma once

#include <IStreamWriter.h>
#include <ThreadPool.h>

#include <iostream>
#include <ostream>

/**
 * @brief Класс вывода блока команд в консоль.
 */
class ConsoleWriter : public IStreamWriter, public ThreadPool<> {
  public:
    /**
     * @brief Консруктор.
     * @param os - поток для вывода.
     */
    explicit ConsoleWriter(std::ostream& os = std::cout) : os_(os) {
    }

    ~ConsoleWriter() override {
      stop();
    }

    /**
     * @brief Запись блока команд в поток.
     * @param context_id - id контекста из которого производится вывод.
     * @param bulk - блок команд.
     */
    void write(uint8_t context_id, const Bulk& bulk) final;

  private:
    std::ostream& os_;
};

