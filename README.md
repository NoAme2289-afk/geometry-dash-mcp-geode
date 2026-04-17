# Geometry Dash MCP Server 🎮

> MCP сервер для создания уровней Geometry Dash с помощью AI в реальном времени!

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![TypeScript](https://img.shields.io/badge/TypeScript-5.7-blue.svg)](https://www.typescriptlang.org/)
[![MCP](https://img.shields.io/badge/MCP-1.0-green.svg)](https://modelcontextprotocol.io/)

## ✨ Возможности

- 🤖 **AI-управляемое создание уровней** - просто опишите, что хотите
- ⚡ **Hot-reload** - изменения появляются в игре мгновенно (требует DLL)
- 🎨 **Поддержка объектов** - блоки, шипы, платформы, орбы, порталы
- 📦 **Экспорт/импорт** - совместимость с форматом Geometry Dash
- 🔧 **Расширяемость** - легко добавить новые типы объектов

## 🚀 Быстрый старт

### 1. Установка

Проект уже собран! Просто настройте Claude Desktop:

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

Перезапустите Claude Desktop.

### 2. Создайте первый уровень

Откройте Claude и напишите:

```
Создай уровень "My First Level" со сложностью normal
Добавь 5 блоков в ряд начиная с (0, 0)
Добавь шип на (2, 0)
Экспортируй уровень
```

### 3. Импортируйте в GD

Скопируйте полученную строку уровня и импортируйте в редактор Geometry Dash.

## 📚 Документация

- **[USAGE.md](USAGE.md)** - Полное руководство по использованию
- **[HOT_RELOAD_GUIDE.md](HOT_RELOAD_GUIDE.md)** - Настройка hot-reload
- **[OFFSETS_GUIDE.md](OFFSETS_GUIDE.md)** - Поиск offsets для GD

## 🛠️ Доступные инструменты

| Инструмент | Описание | Пример |
|------------|----------|--------|
| `create_level` | Создать новый уровень | `Создай уровень "Epic Jump"` |
| `add_object` | Добавить объект | `Добавь блок на (5, 3)` |
| `export_level` | Экспортировать уровень | `Экспортируй уровень` |
| `hot_reload` | Загрузить в игру | `Сделай hot reload` |
| `list_levels` | Список уровней | `Покажи все уровни` |

## 🎯 Примеры

### Простой паркур

```
Создай уровень "Easy Parkour" со сложностью easy
Добавь платформы на (0,0), (3,2), (6,4), (9,2), (12,0)
Добавь орб на (5, 5)
Экспортируй уровень
```

### Автоматическая генерация

```
Создай уровень "Auto Level"
Создай лестницу из 10 блоков, каждый на 1 выше предыдущего
Добавь шипы под каждым блоком
Сделай hot reload
```

## 🔥 Hot Reload (Экспериментально)

Hot reload позволяет видеть изменения в игре мгновенно!

**Требования:**
1. Собранный DLL инжектор (`gd-injector/`)
2. Geometry Dash запущен в режиме редактора
3. DLL загружен в процесс GD

**Статус:** Требуется реализация offsets (см. OFFSETS_GUIDE.md)

## 📁 Структура проекта

```
geometry-dash-mcp/
├── src/
│   ├── index.ts          # MCP сервер
│   ├── ipc-client.ts     # IPC для hot-reload
│   ├── types.ts          # Типы данных
│   └── level-encoder.ts  # Кодирование уровней
├── gd-injector/
│   ├── main.cpp          # DLL инжектор
│   └── CMakeLists.txt    # Сборка C++
├── build/                # Скомпилированный код
└── docs/                 # Документация
```

## 🔧 Разработка

```bash
# Пересборка
npm run build

# Режим разработки
npm run watch

# Сборка DLL (требуется CMake)
cd gd-injector
mkdir build && cd build
cmake .. && cmake --build . --config Release
```

## ⚠️ Важные замечания

- **Hot reload** требует reverse engineering GD для поиска offsets
- **Базовый режим** (export_level) работает из коробки
- **Не используйте онлайн** - риск бана аккаунта
- **Тестируйте на копии** игры

## 🤝 Вклад в проект

Помогите проекту:
- 🔍 Найдите offsets для вашей версии GD
- 🐛 Сообщите о багах
- 💡 Предложите новые функции
- 📖 Улучшите документацию

## 📄 Лицензия

MIT License - используйте свободно!

## 🔗 Ресурсы

- [Model Context Protocol](https://modelcontextprotocol.io/)
- [Geode SDK](https://geode-sdk.org/)
- [GD Modding Discord](https://discord.gg/geometrydash)

---

**Создано:** 17 апреля 2026  
**Версия:** 0.1.0  
**Статус:** Beta (hot-reload в разработке)
