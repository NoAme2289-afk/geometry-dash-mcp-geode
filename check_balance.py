"""
Проверка баланса токенов GigaChat
"""

import requests
import uuid


def check_balance(api_key):
    # Получаем токен
    url = "https://ngw.devices.sberbank.ru:9443/api/v2/oauth"
    
    headers = {
        "Authorization": f"Basic {api_key}",
        "RqUID": str(uuid.uuid4()),
        "Content-Type": "application/x-www-form-urlencoded"
    }
    
    data = {"scope": "GIGACHAT_API_PERS"}
    
    try:
        response = requests.post(url, headers=headers, data=data, verify=False)
        response.raise_for_status()
        access_token = response.json()["access_token"]
        print("[OK] Access token received")
        
        # Проверяем баланс
        balance_url = "https://gigachat.devices.sberbank.ru/api/v1/balance"
        
        headers = {
            "Authorization": f"Bearer {access_token}"
        }
        
        response = requests.get(balance_url, headers=headers, verify=False)
        response.raise_for_status()
        
        balance = response.json()
        print("\n" + "=" * 60)
        print("BALANCE INFO:")
        print("=" * 60)
        print(json.dumps(balance, indent=2, ensure_ascii=False))
        print("=" * 60)
        
    except Exception as e:
        print(f"[ERROR] {e}")
        if hasattr(e, 'response'):
            print(f"Response: {e.response.text}")


if __name__ == "__main__":
    import urllib3
    import json
    urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)
    
    API_KEY = "MDE5ZGExNjctYTZiOC03MjZmLThmN2EtMWUwZDQ5MzRlODE1OjBlZDA2MzcwLTQyYTItNDkyYy04NzZkLWZhZDcxOTllMTE4NQ=="
    check_balance(API_KEY)
