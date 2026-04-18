# 🔧 Исправление: MCP серверы не появляются в OpenCode

## Проблема
MCP серверы не отображаются в OpenCode после добавления конфигурации.

## ✅ Проверено
- ✓ Конфигурация правильная: `C:\Users\danys\AppData\Roaming\OpenCode\opencode.json`
- ✓ MCP сервер работает: `node C:\Users\danys\Desktop\geometry-dash-mcp\build\index.js`
- ✓ Путь к файлу корректный

## 🔧 Решение

### Вариант 1: Полная перезагрузка OpenCode (рекомендуется)

1. **Закройте все окна OpenCode**
2. **Откройте Task Manager** (Ctrl+Shift+Esc)
3. **Найдите процесс "OpenCode"** во вкладке "Processes"
4. **Завершите процесс** (End Task)
5. **Подождите 5 секунд**
6. **Запустите OpenCode заново**

### Вариант 2: Через PowerShell

Выполните в PowerShell:

```powershell
# Закрыть OpenCode
Get-Process -Name "OpenCode" -ErrorAction SilentlyContinue | Stop-Process -Force

# Подождать
Start-Sleep -Seconds 3

# Запустить OpenCode (укажите правильный путь)
Start-Process "C:\Users\danys\AppData\Local\Programs\OpenCode\OpenCode.exe"
```

### Вариант 3: Проверка через настройки OpenCode

1. Откройте OpenCode
2. Нажмите **Ctrl+P** (или Cmd+P)
3. Введите: **"Open Settings"**
4. Найдите секцию **"MCP Servers"**
5. Проверьте, что там есть:
   - `blender`
   - `geometry-dash`

### Вариант 4: Ручное редактирование (если ничего не помогло)

1. Закройте OpenCode полностью
2. Откройте файл:
   ```
   C:\Users\danys\AppData\Roaming\OpenCode\opencode.json
   ```
3. Убедитесь что содержимое точно такое:
   ```json
   {
     "$schema": "https://opencode.ai/config.json",
     "mcp": {
       "blender": {
         "type": "local",
         "command": ["npx", "-y", "@iflow-mcp/pranav-deshmukh-blender-mcp"],
         "enabled": true
       },
       "geometry-dash": {
         "type": "local",
         "command": ["node", "C:\\Users\\danys\\Desktop\\geometry-dash-mcp\\build\\index.js"],
         "enabled": true
       }
     }
   }
   ```
4. Сохраните файл
5. Запустите OpenCode

## 🔍 Как проверить что MCP работает

После перезапуска OpenCode:

1. **Откройте чат**
2. **Начните печатать команду**
3. **Должны появиться инструменты:**
   - `create_level`
   - `add_object`
   - `export_level`
   - `hot_reload`
   - `list_levels`

Или просто напишите:
```
Покажи все уровни
```

Если MCP работает, вы получите ответ: "No levels created yet. Use create_level to create one."

## 🐛 Если всё ещё не работает

### Проверка 1: Логи OpenCode

Откройте Developer Tools в OpenCode:
- Windows/Linux: **Ctrl+Shift+I**
- Mac: **Cmd+Option+I**

Перейдите во вкладку **Console** и ищите ошибки связанные с MCP.

### Проверка 2: Тест MCP сервера вручную

Откройте PowerShell и выполните:

```powershell
node "C:\Users\danys\Desktop\geometry-dash-mcp\build\index.js"
```

Должно появиться:
```
Geometry Dash MCP Server running on stdio
```

Если появляется ошибка - MCP сервер не работает.

### Проверка 3: Права доступа

Убедитесь что у вас есть права на чтение файла:

```powershell
Get-Acl "C:\Users\danys\Desktop\geometry-dash-mcp\build\index.js" | Format-List
```

## 📞 Альтернативный способ использования

Если MCP не появляется в OpenCode, можете использовать **базовый режим**:

1. Откройте PowerShell
2. Запустите MCP сервер вручную:
   ```powershell
   node "C:\Users\danys\Desktop\geometry-dash-mcp\build\index.js"
   ```
3. Используйте через другой MCP клиент (например, Claude Desktop)

## ✅ Ожидаемый результат

После правильной настройки в OpenCode должны появиться:

**MCP Servers:**
- 🎨 blender
- 🎮 geometry-dash

**Tools (geometry-dash):**
- create_level
- add_object
- export_level
- hot_reload
- list_levels

---

**Важно:** OpenCode должен быть **полностью закрыт** перед перезапуском, иначе изменения конфигурации не применятся!
