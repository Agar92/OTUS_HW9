#pragma once

#include <CmdProcessContext.h>
#include <async.h>

#include <mutex>
#include <map>
#include <set>

/**
 * @brief Класс обработчика команд.
 */
class CmdProcessor  {
  public:
    /**
     * @brief Дать объект класса CmdProcessor.
     * @return объект класса CmdProcessor.
     */
    static CmdProcessor& get_instance();

    /**
     * @brief Создать контекст обработки блоков команд.
     * @param bulk_size - размер блока команд.
     * @return handle контекста обработки команд.
     */
    handle_t create_context(size_t bulk_size);

    /**
     * @brief Уничтожить контекст обработки блоков команд.
     * @param handle - handle контекста обработки команд.
     */
    void destroy_context(const handle_t& handle);

    /**
     * @brief Обработать входные данные, содержащие команды.
     * @param handle - handle контекста обработки команд.
     * @param data - обрабатываемые данные.
     * @param size - размер данных.
     */
    void process(const handle_t& handle, const char* data, std::size_t size);

  private:
    CmdProcessor() = default;
    ~CmdProcessor() = default;

    std::mutex contexts_mutex_{};
    std::map<handle_t, std::shared_ptr<CmdProcessContext>> contexts_;
    uint8_t context_id_{};
};

