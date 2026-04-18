# 🎉 Готово! Geometry Dash MCP с Geode SDK

## ✅ Что создано:

1. **Приватный GitHub репозиторий:** https://github.com/NoAme2289-afk/geometry-dash-mcp-geode
2. **Geode мод** с полной интеграцией
3. **GitHub Actions** для автоматической сборки
4. **Dev Container** для разработки в Codespaces

## 🚀 Как использовать:

### Шаг 1: Откройте Codespace

1. Перейдите: https://github.com/NoAme2289-afk/geometry-dash-mcp-geode
2. Нажмите **Code** → **Codespaces** → **Create codespace on main**
3. Подождите пока контейнер создастся (~2-3 минуты)

### Шаг 2: Соберите мод в Codespace

В терминале Codespace выполните:

```bash
cd geode-mod
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Или просто дождитесь автоматической сборки через GitHub Actions!

### Шаг 3: Скачайте собранный мод

**Вариант A: Из GitHub Actions**
1. Перейдите в **Actions** → последний workflow
2. Скачайте **Artifacts** → `gd-mcp-mod-Windows`
3. Распакуйте `.geode` файл

**Вариант B: Из Codespace**
1. Найдите файл `geode-mod/build/noame2289.gdmcp.geode`
2. Скачайте его (правый клик → Download)

### Шаг 4: Установите мод

1. Установите Geode: https://geode-sdk.org/install
2. Скопируйте `.geode` файл в:
   ```
   C:\Users\danys\AppData\Local\GeometryDash\geode\mods\
   ```
3. Запустите Geometry Dash

### Шаг 5: Используйте!

1. Откройте редактор уровней в GD
2. В OpenCode напишите:

```
Создай уровень "AI Level"
Добавь блок на (0, 0)
Добавь блок на (1, 0)
Добавь шип на (2, 0)
Сделай hot reload
```

**Объекты появятся в редакторе мгновенно!** 🎮

## 📁 Структура проекта:

```
geometry-dash-mcp-geode/
├── .github/
│   └── workflows/
│       └── build.yml          # Автоматическая сборка
├── .devcontainer/
│   ├── Dockerfile             # Docker контейнер
│   └── devcontainer.json      # Настройки Codespace
├── geode-mod/
│   ├── src/
│   │   └── main.cpp           # Geode мод (C++)
│   ├── mod.json               # Метаданные мода
│   └── CMakeLists.txt         # Сборка
├── src/                       # MCP сервер (TypeScript)
└── README_GEODE.md            # Документация
```

## 🔧 Преимущества Geode:

✅ **Готовые API** - не нужно искать offsets  
✅ **Автообновление** - работает с новыми версиями GD  
✅ **Стабильность** - используется всем сообществом  
✅ **Простота** - обычный C++ код  
✅ **Безопасность** - официальный мод-лоадер  

## 📝 Что дальше:

1. **Откройте Codespace** и соберите мод
2. **Скачайте .geode файл**
3. **Установите в GD**
4. **Тестируйте!**

## 🐛 Если что-то не работает:

- Проверьте что Geode установлен
- Убедитесь что мод включен в Geode меню
- Проверьте логи: `geode/logs/`
- Откройте Issue на GitHub

---

**Репозиторий:** https://github.com/NoAme2289-afk/geometry-dash-mcp-geode  
**Статус:** ✅ Готово к сборке в Codespaces!  
**Дата:** 18 апреля 2026
