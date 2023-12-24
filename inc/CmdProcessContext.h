#pragma once

#include <Bulk.h>
#include <CmdInterpreter.h>
#include <IObservable.h>

#include <functional>
#include <vector>
#include <memory>
#include <filesystem>

/**
 * @brief Контекст обработчика команд
 */
class CmdProcessContext : public IObservable  {
  public:
    /**
     * @brief Конструктор
     * @param bulk_size - максимальный размер блока команд
     */
    explicit CmdProcessContext(size_t bulk_size, uint8_t id) :
      interpreter_{bulk_size}, id_{id} {
        //создаём директорию (если её нет), куда помещаются генерируемые лог-файлы:
        std::filesystem::create_directory(std::filesystem::path("log"));
    }

    /**
     * @brief Подписка на получение блоков команд на вывод
     * @param observer - подписчик/наблюдатель
     */
    void subscribe(std::unique_ptr<IStreamWriter> observer) final;

    /**
     * @brief Обработка входной команды
     * @param data - входной данные, содержащие команды
     * @param size - размер входных данных
     * @param finish_bulk - принудительное завершение блока команд
     */
    void process(const char* data, std::size_t size, bool finish_bulk = false);

  private:
    /**
     * @brief Публикация блока команд
     * @param bulk - блок команд
     */
    void publish(const Bulk& bulk) final;


    /// Список наблюдателей, ожидающих вывод содержимого пула
    std::vector<std::unique_ptr<IStreamWriter>> observers_{};
    /// Блок команд
    Bulk bulk_{};
    /// Интерпретатор команд
    CmdInterpreter interpreter_;
    /// Строка, содержащая данные с входными командами
    std::string data_;
    /// id контекста
    uint8_t id_;
};

