# Geometry Dash MCP Server - Hot Reload Setup

## Архитектура

```
AI (Claude) 
    ↓ MCP Protocol
MCP Server (TypeScript)
    ↓ Named Pipe IPC
Injector DLL (C++)
    ↓ Memory Injection
Geometry Dash Process
```

## Установка

### 1. Установка MCP сервера

```bash
cd C:\Users\danys\Desktop\geometry-dash-mcp
npm install
npm run build
```

### 2. Сборка инжектора (требуется Visual Studio или MinGW)

```bash
cd gd-injector
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### 3. Настройка Claude Desktop

Добавьте в `%APPDATA%\Claude\claude_desktop_config.json`:

```json
{
  "mcpServers": {
    "geometry-dash": {
      "command": "node",
      "args": ["C:\\Users\\danys\\Desktop\\geometry-dash-mcp\\build\\index.js"]
    }
  }
}
```

## Использование Hot Reload

### Шаг 1: Запустите Geometry Dash

Откройте Geometry Dash и зайдите в редактор уровней.

### Шаг 2: Загрузите инжектор

Используйте любой DLL инжектор для загрузки `gd-mcp-injector.dll` в процесс GeometryDash.exe:

```bash
# Пример с использованием инжектора
injector.exe GeometryDash.exe "C:\Users\danys\Desktop\geometry-dash-mcp\gd-injector\bin\gd-mcp-injector.dll"
```

### Шаг 3: Создавайте уровни через AI

```
Создай уровень "Test Level"
Добавь блок на (0, 0)
Добавь блок на (1, 0)
Добавь шип на (2, 0)
Сделай hot reload
```

## Новый инструмент: hot_reload

**Параметры:**
- `levelName` - название уровня для загрузки

**Что происходит:**
1. MCP сервер кодирует уровень
2. Отправляет через Named Pipe в DLL
3. DLL инжектит объекты в редактор GD
4. Уровень появляется в реальном времени!

## Важные замечания

### ⚠️ Требуется доработка

Текущая реализация - это **каркас**. Для полной работы нужно:

1. **Найти offsets Geometry Dash:**
   - Адрес `EditorUI` класса
   - Адрес `LevelEditorLayer` класса
   - Функция создания объектов
   - Функция обновления редактора

2. **Reverse engineering:**
   - Используйте Cheat Engine / IDA Pro / Ghidra
   - Найдите функции создания объектов
   - Определите структуры данных GD

3. **Реализовать в DLL:**
   - Парсинг level string
   - Вызов функций GD для создания объектов
   - Обновление UI редактора

### Альтернативные подходы

Если reverse engineering слишком сложен:

1. **Использовать существующие моды:**
   - Geode (мод-фреймворк для GD)
   - Mega Hack v7
   - Интегрироваться с их API

2. **Работа с файлами:**
   - Записывать уровни в `CCLocalLevels.dat`
   - Перезагружать редактор

## Структура проекта

```
geometry-dash-mcp/
├── src/
│   ├── index.ts           # MCP сервер
│   ├── ipc-client.ts      # IPC клиент (Named Pipe)
│   ├── types.ts           # Типы данных
│   └── level-encoder.ts   # Кодирование уровней
├── gd-injector/
│   ├── main.cpp           # DLL инжектор
│   └── CMakeLists.txt     # Сборка
├── build/                 # Скомпилированный TS
└── README.md
```

## Отладка

### Проверка IPC соединения

DLL создает консоль при загрузке. Вы увидите:
```
[GD-MCP] Injector loaded!
[GD-MCP] Processing level data: ...
```

### Если hot_reload не работает

1. Проверьте, что DLL загружена (консоль должна появиться)
2. Убедитесь, что вы в редакторе уровней
3. Проверьте логи MCP сервера
4. Используйте fallback: `export_level` для ручного импорта

## Следующие шаги

1. Изучите структуру памяти GD с помощью Cheat Engine
2. Найдите функции создания объектов
3. Реализуйте `ProcessLevelData()` в `main.cpp`
4. Добавьте поддержку всех типов объектов GD
5. Реализуйте очистку уровня перед загрузкой

## Ресурсы

- [Geometry Dash Modding Discord](https://discord.gg/geometrydash)
- [Geode SDK](https://github.com/geode-sdk/geode)
- [GD Programming Wiki](https://github.com/Wyliemaster/GD-Programming-Wiki)

## Лицензия

MIT
