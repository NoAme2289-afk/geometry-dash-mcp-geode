"""
Создание тестового изображения для проверки GigaChat Vision
"""

from PIL import Image, ImageDraw, ImageFont

# Создаем простое тестовое изображение
img = Image.new('RGB', (400, 300), color='lightblue')
draw = ImageDraw.Draw(img)

# Рисуем простые фигуры
draw.rectangle([50, 50, 150, 150], fill='red', outline='black', width=3)
draw.ellipse([200, 50, 300, 150], fill='green', outline='black', width=3)
draw.polygon([(100, 200), (150, 250), (50, 250)], fill='yellow', outline='black', width=3)

# Добавляем текст
try:
    draw.text((150, 20), "Test Image", fill='black')
except:
    pass

# Сохраняем
img.save('test_image.jpg')
print("Test image created: test_image.jpg")
print("\nNow run:")
print("  python test_gigachat.py test_image.jpg")
