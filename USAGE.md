# Geometry Dash MCP Server - Инструкция по использованию

## 🎮 Что это?

MCP сервер, который позволяет AI (Claude) создавать уровни Geometry Dash в реальном времени!

## 📦 Установка

### Быстрый старт (без hot-reload)

1. Проект уже собран в `C:\Users\danys\Desktop\geometry-dash-mcp`

2. Добавьте в Claude Desktop config (`%APPDATA%\Claude\claude_desktop_config.json`):

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

3. Перезапустите Claude Desktop

4. Готово! Теперь можете создавать уровни:

```
Создай уровень "My Level" со сложностью hard
Добавь 5 блоков в ряд начиная с позиции (0, 0)
Добавь шипы между блоками
Экспортируй уровень
```

## 🔥 Hot Reload (продвинутый режим)

Для работы в реальном времени нужно:

### 1. Собрать DLL инжектор

Требуется Visual Studio 2019+ или MinGW:

```bash
cd C:\Users\danys\Desktop\geometry-dash-mcp\gd-injector
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

Результат: `bin/gd-mcp-injector.dll`

### 2. Загрузить DLL в Geometry Dash

**Вариант A: Использовать готовый инжектор**
- Скачайте любой DLL injector (например, Extreme Injector)
- Запустите Geometry Dash
- Инжектите `gd-mcp-injector.dll` в процесс `GeometryDash.exe`

**Вариант B: Использовать существующий мод**
- Установите Geode mod loader
- Создайте мод-обертку для DLL

### 3. Используйте hot_reload

```
Создай уровень "Live Test"
Добавь блок на (0, 0)
Добавь блок на (1, 0)
Сделай hot reload
```

Уровень появится в редакторе мгновенно!

## 🛠️ Доступные команды

### create_level
Создать новый уровень
```
Создай уровень "Epic Jump" со сложностью insane
```

### add_object
Добавить объект в уровень
```
Добавь блок на позицию (5, 3)
Добавь шип на (10, 0)
Добавь платформу на (15, 5)
```

Типы объектов:
- `block` - обычный блок
- `spike` - шип
- `platform` - платформа
- `orb` - орб для прыжков
- `portal` - портал
- `decoration` - декорация

### export_level
Экспортировать уровень (ручной импорт)
```
Экспортируй уровень "My Level"
```

### hot_reload
Загрузить уровень в игру (требует DLL)
```
Сделай hot reload уровня "My Level"
```

### list_levels
Показать все созданные уровни
```
Покажи все уровни
```

## 📝 Примеры использования

### Простой паркур

```
Создай уровень "Easy Parkour" со сложностью easy

Добавь платформу на (0, 0)
Добавь платформу на (3, 2)
Добавь платформу на (6, 4)
Добавь платформу на (9, 2)
Добавь платформу на (12, 0)

Добавь орб на (5, 5)

Экспортируй уровень
```

### Сложный челлендж

```
Создай уровень "Spike Hell" со сложностью demon

Добавь 20 блоков в ряд от (0, 0) до (19, 0)
Добавь шипы на каждую вторую позицию
Добавь орбы на позициях (5, 5), (10, 5), (15, 5)

Сделай hot reload
```

### Автоматическая генерация

```
Создай уровень "Auto Generated"

Создай лестницу из 10 блоков, каждый следующий на 1 выше и правее предыдущего
Добавь шипы под каждым блоком
Добавь декорации для красоты

Экспортируй уровень
```

## ⚠️ Важные замечания

### Текущие ограничения

1. **Hot reload требует доработки** - нужны offsets Geometry Dash
2. **Упрощенный формат** - не все свойства объектов поддерживаются
3. **Нет триггеров** - пока только базовые объекты

### Что нужно для полной работы hot-reload

1. **Reverse engineering GD:**
   - Найти адреса классов EditorUI, LevelEditorLayer
   - Найти функции создания объектов
   - Определить структуры данных

2. **Инструменты:**
   - Cheat Engine
   - IDA Pro / Ghidra
   - x64dbg

3. **Альтернатива:**
   - Использовать Geode SDK
   - Интегрироваться с существующими модами

## 🔧 Разработка

### Пересборка после изменений

```bash
cd C:\Users\danys\Desktop\geometry-dash-mcp
npm run build
```

### Режим разработки

```bash
npm run watch
```

### Структура проекта

```
geometry-dash-mcp/
├── src/
│   ├── index.ts          # Главный MCP сервер
│   ├── ipc-client.ts     # IPC для hot-reload
│   ├── types.ts          # Типы данных
│   └── level-encoder.ts  # Кодирование уровней
├── gd-injector/
│   ├── main.cpp          # DLL инжектор
│   └── CMakeLists.txt
├── build/                # Скомпилированный код
└── README.md
```

## 🎯 Следующие шаги

1. **Для базового использования:**
   - Настройте Claude Desktop
   - Создавайте уровни через export_level
   - Импортируйте вручную в GD

2. **Для hot-reload:**
   - Изучите HOT_RELOAD_GUIDE.md
   - Соберите DLL инжектор
   - Найдите offsets GD
   - Реализуйте ProcessLevelData()

3. **Для продвинутых функций:**
   - Добавьте поддержку триггеров
   - Реализуйте цвета и группы
   - Добавьте импорт существующих уровней

## 📚 Ресурсы

- [Geode SDK](https://github.com/geode-sdk/geode) - мод-фреймворк для GD
- [GD Programming Wiki](https://github.com/Wyliemaster/GD-Programming-Wiki)
- [Geometry Dash Modding Discord](https://discord.gg/geometrydash)

## 🐛 Проблемы?

### MCP сервер не подключается
- Проверьте путь в claude_desktop_config.json
- Перезапустите Claude Desktop
- Проверьте, что проект собран (`npm run build`)

### Hot reload не работает
- Убедитесь, что DLL загружена (должна появиться консоль)
- Проверьте, что вы в редакторе уровней GD
- Используйте fallback: export_level

### Объекты не появляются
- Hot reload требует реализации offsets
- Используйте export_level для ручного импорта

## 📄 Лицензия

MIT

---

**Создано:** 17 апреля 2026  
**Версия:** 0.1.0  
**Статус:** Beta (hot-reload требует доработки)
