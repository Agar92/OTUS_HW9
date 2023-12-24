#pragma once

#include <string>

/**
 * @brief Интерпретатор команд.
 */
class CmdInterpreter {
  public:
    /**
    * @brief Конструктор.
    * @param bulk_size - максимальный размер блока команд.
    */
    explicit CmdInterpreter(size_t bulk_size) : bulk_size_(bulk_size) {
    }

    ~CmdInterpreter() = default;

    /**
     * @brief Обработка принятой команды.
     * @param input - входная команда.
     * @param cmd   - команда для помещения в блок команд.
     * @return true - блок команд завершён, false - блок команд не завершён.
     */
    bool interpret(const std::string& input, std::string& cmd);

  private:

    /// Максимальный размер блока команд.
    size_t bulk_size_{};
    /// Текущий размер число команд в блоке.
    size_t size_{};
    /// Количество не имеющих закрывающей пары символов '{' или '}'.
    size_t tokens_{};
};

