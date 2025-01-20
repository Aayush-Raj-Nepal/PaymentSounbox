const express = require("express");
const cors = require("cors");
const fs = require("fs");

const app = express();
const PORT = process.env.PORT || 3000;
const TRANSACTION_FILE = "transactions.json";

app.use(cors());
app.use(express.json());

// ✅ Load existing transactions or initialize an empty file
if (!fs.existsSync(TRANSACTION_FILE)) {
  fs.writeFileSync(TRANSACTION_FILE, JSON.stringify([]));
}

// ✅ Endpoint to receive payments
app.post("/pay", (req, res) => {
  const { amount } = req.body;
  if (!amount || amount <= 0) {
    return res.status(400).json({ error: "Invalid amount" });
  }

  const transaction = {
    id: Date.now(),
    amount,
    timestamp: new Date().toISOString(),
    viewed: false, // 🚀 Added flag to track whether ESP32 has seen it
  };

  // ✅ Store the transaction
  const transactions = JSON.parse(fs.readFileSync(TRANSACTION_FILE));
  transactions.push(transaction);
  fs.writeFileSync(TRANSACTION_FILE, JSON.stringify(transactions));

  console.log(`✅ Payment received: Rs. ${amount}`);
  res.json({ message: "Payment successful", transaction });
});

// ✅ Endpoint for ESP32 to check for new transactions
app.get("/latest", (req, res) => {
  const transactions = JSON.parse(fs.readFileSync(TRANSACTION_FILE));

  if (transactions.length === 0) {
    return res.json({ newTransaction: false });
  }

  // ✅ Get the latest transaction
  let latestTransaction = transactions[transactions.length - 1];

  if (latestTransaction.viewed) {
    return res.json(0); // If already viewed, return 0
  }

  // ✅ Mark as viewed
  latestTransaction.viewed = true;
  fs.writeFileSync(TRANSACTION_FILE, JSON.stringify(transactions));

  res.json(latestTransaction.amount);
});

app.listen(PORT, () => {
  console.log(`🚀 Server running on http://localhost:${PORT}`);
});
