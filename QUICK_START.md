# 🚀 Быстрый старт - Geometry Dash MCP

## Шаг 1: Настройка MCP сервера в OpenCode

### Добавьте конфигурацию

Откройте настройки OpenCode (Ctrl+P → "Open Settings") и добавьте:

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

### Перезапустите OpenCode

После добавления конфигурации перезапустите OpenCode.

## Шаг 2: Базовое использование (без hot-reload)

Теперь можете создавать уровни через AI:

```
Создай уровень "Test Level" со сложностью normal
Добавь блок на (0, 0)
Добавь блок на (1, 0)
Добавь блок на (2, 0)
Добавь шип на (3, 0)
Экспортируй уровень
```

AI вернет строку уровня - скопируйте её и импортируйте в Geometry Dash вручную.

## Шаг 3: Hot-Reload (продвинутый режим)

### 3.1 Соберите DLL инжектор

Требуется Visual Studio 2019+ или MinGW:

```bash
cd C:\Users\danys\Desktop\geometry-dash-mcp\gd-injector
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

Результат: `bin/gd-mcp-injector.dll`

### 3.2 Запустите Geometry Dash

1. Откройте `C:\Users\danys\Desktop\dffe\Geometry Dash\GeometryDash.exe`
2. Зайдите в редактор уровней (Create → Editor)

### 3.3 Инжектите DLL

**Вариант A: Extreme Injector (рекомендуется)**

1. Скачайте Extreme Injector: https://github.com/master131/ExtremeInjector
2. Запустите Extreme Injector
3. Выберите процесс: `GeometryDash.exe`
4. Выберите DLL: `C:\Users\danys\Desktop\geometry-dash-mcp\gd-injector\bin\gd-mcp-injector.dll`
5. Нажмите "Inject"
6. Должна появиться консоль с текстом:

```
========================================
  Geometry Dash MCP Injector v0.1.0
========================================
[GD-MCP] Injector loaded!
[GD-MCP] Base address: 0x...
[GD-MCP] Bypassing anti-cheat...
[GD-MCP] Anti-cheat bypassed!
[GD-MCP] Starting IPC server...
[GD-MCP] Starting level injection thread...
[GD-MCP] Ready! Waiting for MCP commands...
========================================
```

**Вариант B: Process Hacker**

1. Скачайте Process Hacker: https://processhacker.sourceforge.io/
2. Найдите процесс `GeometryDash.exe`
3. Правый клик → Miscellaneous → Inject DLL
4. Выберите `gd-mcp-injector.dll`

**Вариант C: Xenos Injector**

1. Скачайте Xenos: https://github.com/DarthTon/Xenos
2. Выберите процесс `GeometryDash.exe`
3. Добавьте DLL
4. Нажмите "Inject"

### 3.4 Используйте hot-reload

Теперь в OpenCode:

```
Создай уровень "Live Test"
Добавь блок на (0, 0)
Сделай hot reload
```

Уровень появится в редакторе мгновенно! ⚡

## 📋 Доступные команды

### create_level
```
Создай уровень "Epic Jump" со сложностью insane
```

### add_object
```
Добавь блок на (5, 3)
Добавь шип на (10, 0)
Добавь платформу на (15, 5)
Добавь орб на (20, 10)
```

Типы объектов:
- `block` - блок (ID: 1)
- `spike` - шип (ID: 8)
- `platform` - платформа (ID: 40)
- `orb` - орб (ID: 36)
- `portal` - портал (ID: 12)
- `decoration` - декорация (ID: 211)

### export_level
```
Экспортируй уровень "My Level"
```

### hot_reload
```
Сделай hot reload уровня "My Level"
```

### list_levels
```
Покажи все уровни
```

## 🎯 Примеры

### Простой паркур
```
Создай уровень "Easy Parkour" со сложностью easy
Добавь платформу на (0, 0)
Добавь платформу на (3, 2)
Добавь платформу на (6, 4)
Добавь платформу на (9, 2)
Добавь орб на (5, 5)
Сделай hot reload
```

### Челлендж
```
Создай уровень "Spike Hell" со сложностью demon
Добавь 10 блоков в ряд от (0, 0) до (9, 0)
Добавь шипы на позициях (1, 1), (3, 1), (5, 1), (7, 1), (9, 1)
Добавь орб на (5, 3)
Сделай hot reload
```

## 🔧 Отладка

### MCP сервер не подключается
- Проверьте путь в настройках OpenCode
- Перезапустите OpenCode
- Убедитесь, что проект собран: `npm run build`

### Hot reload не работает
- Проверьте, что консоль DLL появилась
- Убедитесь, что вы в редакторе уровней GD
- Проверьте логи в консоли DLL
- Используйте fallback: `export_level`

### DLL не инжектится
- Запустите инжектор от администратора
- Отключите антивирус временно
- Проверьте, что GD запущен
- Используйте другой инжектор

## ⚠️ Важно

1. **Не используйте онлайн** - риск бана аккаунта
2. **Тестируйте локально** - только в оффлайн режиме
3. **Античит обойден** - DLL автоматически отключает защиту GD
4. **Версия GD**: 2.2 (23 января 2026)

## 📁 Структура файлов

```
C:\Users\danys\Desktop\geometry-dash-mcp\
├── build\
│   └── index.js              # MCP сервер (готов)
├── gd-injector\
│   ├── build\
│   │   └── bin\
│   │       └── gd-mcp-injector.dll  # Инжектор (нужно собрать)
│   └── main.cpp              # Исходный код
└── src\                      # TypeScript исходники

C:\Users\danys\Desktop\dffe\Geometry Dash\
└── GeometryDash.exe          # Ваша игра
```

## 🎓 Что дальше?

1. **Базовый режим работает** - используйте export_level
2. **Для hot-reload** - соберите DLL и инжектите
3. **Экспериментируйте** - создавайте разные уровни
4. **Читайте документацию** - EXAMPLES.md, USAGE.md

## 📚 Документация

- **QUICK_START.md** (этот файл) - быстрый старт
- **USAGE.md** - полное руководство
- **EXAMPLES.md** - примеры команд
- **HOT_RELOAD_GUIDE.md** - детали hot-reload
- **OFFSETS_GUIDE.md** - reverse engineering

---

**Версия:** 0.1.0  
**Дата:** 17 апреля 2026  
**Статус:** Готов к использованию! 🎉
