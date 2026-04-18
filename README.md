# Geometry Dash MCP Server 🎮

> Полнофункциональная AI-система для создания, анализа и визуализации уровней Geometry Dash!

<img src="https://raw.githubusercontent.com/NoAme2289-afk/geometry-dash-mcp-geode/main/preview.png" alt="Level Preview" width="600" />

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Geode](https://img.shields.io/badge/Geode-3.0-blue.svg)](https://geode-sdk.org/)
[![MCP](https://img.shields.io/badge/MCP-1.0-green.svg)](https://modelcontextprotocol.io/)

## 🌟 Особенности

Это **революционная система** для разработки уровней Geometry Dash с использованием AI:

- 🤖 **AI-управляемое создание** - создавайте уровни через естественный язык
- 🎨 **30+ команд** - полный контроль над объектами, триггерами, группами
- 📊 **Экспорт в JSON** - сохраняйте и анализируйте уровни
- 🖼️ **Рендер в PNG** - визуализация уровней с сеткой
- 🧠 **AI анализ** - получайте рекомендации по улучшению дизайна
- ⚡ **Real-time** - изменения применяются мгновенно через named pipe
- 🔧 **Geode мод** - нативная интеграция с Geometry Dash

## 🎯 Что можно делать

- ✅ Создавать объекты (блоки, шипы, порталы, орбы)
- ✅ Настраивать триггеры (Move, Rotate, Scale, Alpha, Color, Pulse)
- ✅ Управлять группами и цветами
- ✅ Batch операции (копирование, масштабирование, вращение)
- ✅ Экспортировать уровни в JSON
- ✅ Рендерить визуализацию в PNG
- ✅ Получать AI анализ играбельности и дизайна
- ✅ Итеративно улучшать уровни на основе AI рекомендаций

## 🚀 Быстрый старт

### 1. Установка Geode мода

1. Скачайте последнюю версию мода из [Releases](https://github.com/NoAme2289-afk/geometry-dash-mcp-geode/releases)
2. Скопируйте `gdmcp.geode` в папку с модами Geode:
   - **Стандартный путь:** `%LOCALAPPDATA%\GeometryDash\geode\mods\`
   - **Или в папку с игрой:** `<путь к GD>\geode\mods\`
3. Запустите Geometry Dash

### 2. Установка Python MCP сервера

```bash
pip install -e .
```

### 3. Настройка OpenCode

Добавьте в конфигурацию OpenCode (`opencode.json`):

```json
{
  "mcpServers": {
    "geometry-dash": {
      "command": "python",
      "args": ["-m", "mcp_server_gd"]
    }
  }
}
```

Перезапустите OpenCode.

### 4. Создайте первый уровень

Откройте редактор уровня в Geometry Dash, затем в OpenCode напишите:

```
Create a playable Geometry Dash level with platforms, spikes, and an orb
```

## 🤖 Рекомендуемые AI модели

GDMCP лучше всего работает с продвинутыми языковыми моделями:

- **Claude Opus 4.7** - отличные результаты ✅
- **Claude Opus 4.6** - отличные результаты ✅
- **Claude Sonnet 4.6** - хорошие результаты ✅
- **GPT 5.4** - хорошие результаты ✅
- **И другие современные модели**

Эти модели хорошо понимают контекст создания уровней и дают качественные рекомендации по дизайну.

## 🎮 Пример профессионального уровня

**Автор:** Viteral666 (aka Okira)

Этот репозиторий был протестирован на реальном профессиональном уровне:
- **17,969 объектов** - 4 года разработки
- **1,434 триггера** - сложная анимация и эффекты
- **401 Color Trigger** - динамическая цветовая палитра
- **384 Rotate Trigger** - вращающиеся элементы
- **189 Shader Trigger** - продвинутые визуальные эффекты

Система успешно экспортировала, визуализировала и проанализировала этот уровень!

## 🛠️ Доступные инструменты

### Создание объектов
- `gd_create_object` - Создать объект с группами и цветами
- `gd_delete_all` - Удалить все объекты
- `gd_delete_objects` - Удалить объекты по группе
- `gd_move_objects` - Переместить группу объектов

### Триггеры
- `gd_move_trigger` - Move Trigger (движение объектов)
- `gd_alpha_trigger` - Alpha Trigger (прозрачность)
- `gd_rotate_trigger` - Rotate Trigger (вращение)
- `gd_scale_trigger` - Scale Trigger (масштабирование)

### Анализ и экспорт
- `gd_get_level_info` - Получить статистику уровня
- `gd_list_objects` - Список первых 50 объектов
- `gd_export_level` - Экспорт уровня в JSON
- `gd_render_level` - Рендер уровня в PNG с сеткой

### Управление
- `gd_undo` - Отменить последнее действие
- `gd_redo` - Повторить отменённое действие

## 🎯 Примеры использования

### Создание простого уровня

```
Create 5 blocks at ground level (Y=105) starting from X=100
Add a spike at X=200, Y=135
Add a yellow orb at X=300, Y=200
Export and render the level
```

### Добавление анимации

```
Create a platform at X=500, Y=105 in group 1
Add a Move Trigger to move group 1 up by 100 units over 2 seconds
Add a Rotate Trigger to rotate group 1 by 360 degrees
```

### Анализ существующего уровня

```
Get level info from Geometry Dash
Export the current level to JSON
Render the level to PNG
```

## 🔥 AI-управляемая разработка уровней

Эта система демонстрирует **будущее разработки игр**:

1. **Создание через естественный язык** - просто опишите что хотите
2. **AI анализ** - получайте профессиональные рекомендации
3. **Итеративное улучшение** - улучшайте дизайн на основе AI фидбека
4. **Визуализация** - видите результат до тестирования в игре

### Процесс разработки с AI

```
1. Создайте базовый уровень
   → "Create a simple platformer level"

2. Экспортируйте и визуализируйте
   → "Export and render the level"

3. Получите AI анализ (через GigaChat MCP)
   → Анализ играбельности, баланса, дизайна

4. Улучшите на основе рекомендаций
   → "Add more platforms to fill the gaps"

5. Повторяйте до достижения 9/10!
```

## 📁 Структура проекта

```
geometry-dash-mcp/
├── geode-mod/              # Geode мод для Geometry Dash
│   ├── src/
│   │   ├── main.cpp        # Основной код мода
│   │   ├── CommandHandlers.hpp  # Обработчики команд
│   │   └── MCPPanel.hpp    # UI панель в игре
│   └── mod.json            # Метаданные мода
├── mcp_server_gd/          # Python MCP сервер
│   ├── __init__.py         # MCP инструменты
│   └── renderer.py         # Рендерер PNG
├── .github/workflows/      # GitHub Actions для сборки
└── README.md               # Эта документация
```

## 🔧 Разработка

### Сборка Geode мода

Мод собирается автоматически через GitHub Actions при каждом push.

Для локальной сборки:
```bash
cd geode-mod
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### Разработка Python сервера

```bash
# Установка в режиме разработки
pip install -e .

# Тестирование
python -m mcp_server_gd
```

## ⚠️ Важные замечания

- **Требуется Geode** - мод работает только с Geode SDK 3.0+
- **Только для редактора** - не используйте в онлайн режиме
- **Резервные копии** - регулярно экспортируйте уровни в JSON
- **Тестирование** - всегда тестируйте уровни перед публикацией

## 🤝 Вклад в проект

Помогите улучшить проект:
- 🐛 Сообщайте о багах через Issues
- 💡 Предлагайте новые функции
- 📖 Улучшайте документацию
- ⭐ Ставьте звезду если проект полезен!

## 🎨 Благодарности

**Тестирование на профессиональном уровне:**
- **Viteral666 (aka Okira)** - автор тестового уровня (17,969 объектов, 4 года разработки)

**Технологии:**
- [Geode SDK](https://geode-sdk.org/) - модификация Geometry Dash
- [Model Context Protocol](https://modelcontextprotocol.io/) - AI интеграция
- [FastMCP](https://github.com/jlowin/fastmcp) - Python MCP framework
- [GigaChat](https://developers.sber.ru/gigachat) - AI анализ уровней

## 📄 Лицензия

MIT License - используйте свободно!

## 🔗 Ресурсы

- [Geode SDK Documentation](https://docs.geode-sdk.org/)
- [MCP Documentation](https://modelcontextprotocol.io/docs)
- [Geometry Dash Modding Discord](https://discord.gg/geometrydash)

---

**Создано:** 18 апреля 2026  
**Версия:** 1.0.2  
**Статус:** Production Ready ✅

**Достижения:**
- ✅ 30+ команд для управления уровнями
- ✅ Экспорт в JSON и рендер в PNG
- ✅ AI анализ через GigaChat
- ✅ Протестировано на уровне с 17,969 объектами
- ✅ Итеративное улучшение дизайна (от 0/10 до 9/10)
