"""
GigaChat Vision API - Правильная работа с изображениями
Использует метод загрузки файлов
"""

import requests
import json
import os
import uuid


class GigaChatVision:
    def __init__(self, api_key):
        self.api_key = api_key
        self.base_url = "https://gigachat.devices.sberbank.ru/api/v1"
        self.access_token = None
        
    def get_access_token(self):
        """Получить access token"""
        url = "https://ngw.devices.sberbank.ru:9443/api/v2/oauth"
        
        headers = {
            "Authorization": f"Basic {self.api_key}",
            "RqUID": str(uuid.uuid4()),
            "Content-Type": "application/x-www-form-urlencoded"
        }
        
        data = {"scope": "GIGACHAT_API_PERS"}
        
        try:
            response = requests.post(url, headers=headers, data=data, verify=False)
            response.raise_for_status()
            self.access_token = response.json()["access_token"]
            print(f"[OK] Access token received")
            return self.access_token
        except Exception as e:
            print(f"[ERROR] Token error: {e}")
            return None
    
    def upload_file(self, file_path):
        """Загрузить файл в GigaChat"""
        if not self.access_token:
            self.get_access_token()
        
        url = f"{self.base_url}/files"
        
        headers = {
            "Authorization": f"Bearer {self.access_token}"
        }
        
        # Определяем MIME тип
        ext = os.path.splitext(file_path)[1].lower()
        mime_types = {
            '.jpg': 'image/jpeg',
            '.jpeg': 'image/jpeg',
            '.png': 'image/png',
            '.gif': 'image/gif',
            '.webp': 'image/webp'
        }
        mime_type = mime_types.get(ext, 'image/jpeg')
        
        try:
            with open(file_path, 'rb') as f:
                files = {
                    'file': (os.path.basename(file_path), f, mime_type)
                }
                
                data = {
                    'purpose': 'general'
                }
                
                print(f"[...] Uploading file: {file_path}")
                response = requests.post(url, headers=headers, files=files, data=data, verify=False)
                response.raise_for_status()
                
                file_id = response.json()["id"]
                print(f"[OK] File uploaded, ID: {file_id}")
                return file_id
                
        except Exception as e:
            print(f"[ERROR] Upload failed: {e}")
            if hasattr(e, 'response'):
                print(f"Response: {e.response.text}")
            return None
    
    def analyze_image(self, image_path, prompt="Podrobno opishi chto izobrazheno na etoy kartinke"):
        """Анализировать изображение"""
        if not self.access_token:
            self.get_access_token()
        
        # Загружаем файл
        file_id = self.upload_file(image_path)
        if not file_id:
            return "Failed to upload file"
        
        # Отправляем запрос с файлом
        url = f"{self.base_url}/chat/completions"
        
        headers = {
            "Authorization": f"Bearer {self.access_token}",
            "Content-Type": "application/json"
        }
        
        payload = {
            "model": "GigaChat-Max",
            "messages": [
                {
                    "role": "user",
                    "content": prompt,
                    "attachments": [file_id]
                }
            ],
            "temperature": 0.7
        }
        
        try:
            print("[...] Sending request to GigaChat...")
            response = requests.post(url, headers=headers, json=payload, verify=False)
            response.raise_for_status()
            
            result = response.json()
            answer = result["choices"][0]["message"]["content"]
            return answer
            
        except requests.exceptions.HTTPError as e:
            if e.response.status_code == 401:
                print("[...] Token expired, getting new one...")
                self.get_access_token()
                return self.analyze_image(image_path, prompt)
            else:
                return f"HTTP Error: {e}\nResponse: {e.response.text}"
        except Exception as e:
            return f"Error: {e}"


def main():
    import sys
    
    API_KEY = "MDE5ZGExNjctYTZiOC03MjZmLThmN2EtMWUwZDQ5MzRlODE1OjBlZDA2MzcwLTQyYTItNDkyYy04NzZkLWZhZDcxOTllMTE4NQ=="
    
    if len(sys.argv) > 1:
        image_path = sys.argv[1]
    else:
        image_path = "703a79ab57ffe64f7043e714d08f98bd.jpg"
    
    if not os.path.exists(image_path):
        print(f"[ERROR] File not found: {image_path}")
        return
    
    print("=" * 60)
    print("GigaChat Vision - Image Analysis (File Upload Method)")
    print("=" * 60)
    print(f"Image: {image_path}\n")
    
    client = GigaChatVision(API_KEY)
    
    result = client.analyze_image(
        image_path,
        prompt="Podrobno opishi chto izobrazheno na etoy kartinke"
    )
    
    print("\n" + "=" * 60)
    print("RESULT:")
    print("=" * 60)
    print(result)
    print("=" * 60)


if __name__ == "__main__":
    import urllib3
    urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)
    main()
