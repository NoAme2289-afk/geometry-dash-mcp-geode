# Поиск Offsets для Geometry Dash - Руководство

## 🎯 Цель

Найти адреса памяти и функции в Geometry Dash для реализации hot-reload.

## 🔍 Что нужно найти

### 1. Классы
- `EditorUI` - интерфейс редактора
- `LevelEditorLayer` - слой редактора уровней
- `GameObject` - базовый класс объектов
- `EditorPauseLayer` - меню паузы редактора

### 2. Функции
- `GameObject::create(int objectID)` - создание объекта
- `GameObject::setPosition(float x, float y)` - установка позиции
- `LevelEditorLayer::addObject(GameObject*)` - добавление объекта в уровень
- `LevelEditorLayer::removeAllObjects()` - очистка уровня

### 3. Переменные
- Указатель на текущий `LevelEditorLayer`
- Массив объектов уровня
- Текущая позиция камеры

## 🛠️ Инструменты

### Cheat Engine (рекомендуется для начинающих)

1. **Скачайте:** https://www.cheatengine.org/
2. **Запустите GD** и откройте редактор уровней
3. **Attach к процессу** GeometryDash.exe
4. **Поиск значений:**

```
Шаг 1: Создайте объект на позиции X=100
Шаг 2: Scan Type: Float, Value: 100
Шаг 3: Переместите объект на X=200
Шаг 4: Next Scan, Value: 200
Шаг 5: Повторяйте до 1-10 адресов
```

5. **Найдите базовый адрес:**
   - Правый клик на адрес → "Find out what accesses this address"
   - Запишите инструкции (например: `mov [rax+0x10], xmm0`)

### IDA Pro / Ghidra (для продвинутых)

1. **Загрузите GeometryDash.exe** в дизассемблер
2. **Ищите строки:**
   - "EditorUI"
   - "LevelEditorLayer"
   - "GameObject"
3. **Найдите конструкторы классов**
4. **Анализируйте vtable** (виртуальные таблицы)

### x64dbg (для отладки)

1. **Attach к GD**
2. **Ставьте breakpoints** на подозрительные функции
3. **Анализируйте стек вызовов**

## 📝 Пример поиска

### Поиск GameObject::setPosition

```cpp
// В Cheat Engine:
1. Создайте объект в редакторе
2. Найдите его X координату (Float scan)
3. "Find out what writes to this address"
4. Переместите объект
5. Запишите адрес функции

// Результат может выглядеть так:
GeometryDash.exe+1A2B3C - mov [rcx+10],xmm0  // X position
GeometryDash.exe+1A2B40 - mov [rcx+14],xmm1  // Y position
```

### Поиск LevelEditorLayer

```cpp
// В IDA Pro:
1. Strings window → найдите "LevelEditorLayer"
2. Xrefs → найдите где используется
3. Найдите конструктор класса
4. Запишите offset vtable

// Пример:
.text:00401000 LevelEditorLayer::init
.text:00401010 mov rax, offset LevelEditorLayer_vtable
```

## 🔧 Реализация в DLL

После нахождения offsets, обновите `gd-injector/main.cpp`:

```cpp
// Offsets (замените на реальные!)
#define OFFSET_LEVEL_EDITOR_LAYER 0x12345678
#define OFFSET_CREATE_OBJECT 0x87654321
#define OFFSET_ADD_OBJECT 0x11223344

// Типы функций
typedef void* (__thiscall* CreateObjectFunc)(int objectID);
typedef void (__thiscall* AddObjectFunc)(void* layer, void* object);
typedef void (__thiscall* SetPositionFunc)(void* object, float x, float y);

void ProcessLevelData(const std::string& levelData) {
    // Получить базовый адрес GD
    HMODULE gdModule = GetModuleHandleA("GeometryDash.exe");
    if (!gdModule) return;

    // Получить указатель на LevelEditorLayer
    void* editorLayer = *(void**)((uintptr_t)gdModule + OFFSET_LEVEL_EDITOR_LAYER);
    if (!editorLayer) return;

    // Получить функции
    CreateObjectFunc createObject = (CreateObjectFunc)((uintptr_t)gdModule + OFFSET_CREATE_OBJECT);
    AddObjectFunc addObject = (AddObjectFunc)((uintptr_t)gdModule + OFFSET_ADD_OBJECT);
    SetPositionFunc setPosition = (SetPositionFunc)((uintptr_t)gdModule + OFFSET_SET_POSITION);

    // Парсинг levelData (упрощенно)
    // Формат: "objectID,x,y;objectID,x,y;..."
    
    std::stringstream ss(levelData);
    std::string token;
    
    while (std::getline(ss, token, ';')) {
        int objID;
        float x, y;
        
        if (sscanf(token.c_str(), "%d,%f,%f", &objID, &x, &y) == 3) {
            // Создать объект
            void* obj = createObject(objID);
            if (obj) {
                // Установить позицию
                setPosition(obj, x, y);
                // Добавить в уровень
                addObject(editorLayer, obj);
            }
        }
    }
}
```

## 🎓 Альтернативный подход: Geode SDK

Вместо поиска offsets вручную, используйте Geode:

### 1. Установите Geode

```bash
# Скачайте с https://geode-sdk.org/
# Установите в папку GD
```

### 2. Создайте мод

```bash
geode new my-mcp-mod
cd my-mcp-mod
```

### 3. Используйте Geode API

```cpp
#include <Geode/Geode.hpp>
using namespace geode::prelude;

// Geode предоставляет готовые функции!
void loadLevel(const std::string& levelData) {
    auto editor = LevelEditorLayer::get();
    if (!editor) return;

    // Создание объектов через Geode API
    auto obj = GameObject::create(1); // ID 1 = block
    obj->setPosition({100.0f, 100.0f});
    editor->addChild(obj);
}
```

## 📊 Таблица известных offsets

| Версия GD | LevelEditorLayer | GameObject::create | Примечания |
|-----------|------------------|-------------------|------------|
| 2.2       | ?                | ?                 | Требуется поиск |
| 2.1       | ?                | ?                 | Требуется поиск |

**Помогите проекту:** Если найдете offsets, создайте issue на GitHub!

## 🔗 Полезные ресурсы

- [GD Programming Wiki](https://github.com/Wyliemaster/GD-Programming-Wiki)
- [Geode Documentation](https://docs.geode-sdk.org/)
- [GD Modding Discord](https://discord.gg/geometrydash)
- [Cheat Engine Tutorial](https://www.cheatengine.org/tutorial.php)

## ⚠️ Важно

1. **Offsets меняются** с каждым обновлением GD
2. **Используйте pattern scanning** для надежности
3. **Тестируйте на чистой копии** GD
4. **Не используйте в онлайн режиме** (риск бана)

## 🚀 Быстрый старт без offsets

Пока не найдены offsets, используйте:

1. **Базовый режим:** export_level → ручной импорт
2. **Файловый режим:** запись в CCLocalLevels.dat
3. **Geode мод:** интеграция через готовый API

---

**Статус:** Требуется community помощь для поиска offsets  
**Сложность:** Средняя (Cheat Engine) / Высокая (IDA Pro)  
**Время:** 2-8 часов в зависимости от опыта
