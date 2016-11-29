db.mycoll.drop();

db.mycoll.insert({"x": 1});
db.mycoll.insert({"item": "card", "qty": 15});
db.mycoll.insert({"user": "123xyz",
            "contact": { "phone": "123-456-7890", "email": "xyz@example.com"},
            "access": { "level": 5, "group": "dev"}});
db.mycoll.insert({"date": new Date("2016-05-18T16:00:00Z"), "float": 0.12345});

