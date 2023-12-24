#pragma once

#include <ctime>
#include <ostream>
#include <string>
#include <vector>

/// Класс блока команд.
class Bulk {
  public:
    Bulk() = default;
    ~Bulk() = default;

    /**
     * @brief Поместить команду в блок.
     * @param cmd - команда.
     */
    void push(const std::string& cmd);

    /// Очистка блока команд.
    void clear();

    /// Вернуть размер блока команд.
    size_t size() const;

    /// Вернуть все команды блока.
    std::vector<std::string> get_cmds() const;

    /**
     * @brief Вернуть время начала формирования блока команд.
     * @return время начала формирования блока команд.
     */
    std::time_t time() const;

    /**
     * @brief Оператор вывода в поток блока команд.
     * @param os - поток вывода.
     * @param bulk - блок команд для вывода в поток.
     */
    friend std::ostream& operator <<(std::ostream& os, const Bulk& bulk);

  private:
    /// Пул команд.
    std::vector<std::string> pool_{};
    /// Время начала формирования блока команд.
    std::time_t time_{};
};

