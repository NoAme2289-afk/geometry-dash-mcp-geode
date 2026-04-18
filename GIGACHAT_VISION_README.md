# GigaChat Vision MCP Server

MCP сервер для анализа изображений через GigaChat API.

## Установка

MCP сервер уже добавлен в `opencode.json`.

## Доступные инструменты

### 1. analyze_image
Анализирует изображение с помощью GigaChat-Max.

**Параметры:**
- `image_path` (обязательный): Путь к файлу изображения
- `prompt` (опциональный): Вопрос или инструкция об изображении

**Пример использования в Kiro:**
```
Проанализируй изображение C:\Users\danys\Desktop\image.jpg
```

### 2. check_balance
Проверяет баланс токенов GigaChat API.

**Пример использования в Kiro:**
```
Проверь баланс GigaChat
```

## Поддерживаемые форматы

- JPG/JPEG
- PNG
- GIF
- WEBP

## Баланс токенов

Текущий баланс:
- GigaChat: 900,000 токенов
- GigaChat-Max: 50,000 токенов (используется для vision)

## Примеры

### Анализ изображения
```python
# Через MCP
analyze_image(
    image_path="703a79ab57ffe64f7043e714d08f98bd.jpg",
    prompt="Что изображено на картинке?"
)
```

### Проверка баланса
```python
check_balance()
```

## Технические детали

- **Модель**: GigaChat-Max (поддерживает vision)
- **API**: GigaChat REST API
- **Метод**: Загрузка файла + attachments
- **Лимиты**: До 80 МБ на запрос

## Перезапуск Kiro

После добавления MCP сервера перезапустите Kiro/OpenCode для применения изменений.
