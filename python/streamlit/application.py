# app.py
import streamlit as st

st.title("経費申請フォーム")

# 入力項目
name = st.text_input("申請者名")
department = st.selectbox("部署", ["営業部", "開発部", "総務部"])
amount = st.number_input("申請金額", min_value=0)
reason = st.text_area("申請理由")

# 申請ボタン
if st.button("申請する"):
    st.success(f"{name} さんの申請を受け付けました。")
