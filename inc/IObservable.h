#pragma once

#include <IStreamWriter.h>

#include <memory>

/**
 * @brief Интерфейс наблюдаемого.
 */
class IObservable {
  public:
    virtual ~IObservable() = default;

    /**
     * @brief Оформление подписчиком/наблюдателем подписки на получение блоков команд на вывод.
     * @param observer - подписчик/наблюдатель.
     */
    virtual void subscribe(std::unique_ptr<IStreamWriter> observer) = 0;

    /**
     * @brief Публикация блока команд.
     * @param bulk - блок команд.
     */
    virtual void publish(const Bulk& bulk) = 0;
};

