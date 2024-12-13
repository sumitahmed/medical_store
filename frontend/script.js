// Function to handle connection to the server
async function connectToServer() {
    try {
        let response = await fetch('http://127.0.0.1:8080');  // Adjust the URL and port if needed

        if (response.ok) {
            let data = await response.json();
            document.getElementById('response').innerText = data.message;
            alert("Connected to server!");
        } else {
            console.error("Failed to fetch:", response.statusText);
            alert("Error connecting to server: " + response.statusText);
        }
    } catch (error) {
        console.error("Error connecting to server:", error);
        alert("Error connecting to server: " + error.message);
    }
}

// Function to handle inserting a medicine
async function insertMedicine() {
    const id = prompt("Enter Medicine ID:");
    const name = prompt("Enter Medicine Name:");
    const price = prompt("Enter Medicine Price:");

    const data = { action: "insert", id, name, price };

    try {
        let response = await fetch('http://127.0.0.1:8080', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        });

        if (response.ok) {
            alert("Medicine inserted successfully!");
        } else {
            console.error("Failed to insert medicine:", response.statusText);
            alert("Failed to insert medicine.");
        }
    } catch (error) {
        console.error("Error inserting medicine:", error);
        alert("Error inserting medicine.");
    }
}

// Function to handle deleting a medicine
async function deleteMedicine() {
    const id = prompt("Enter Medicine ID to delete:");

    const data = { action: "delete", id };

    try {
        let response = await fetch('http://127.0.0.1:8080', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        });

        if (response.ok) {
            alert("Medicine deleted successfully!");
        } else {
            console.error("Failed to delete medicine:", response.statusText);
            alert("Failed to delete medicine.");
        }
    } catch (error) {
        console.error("Error deleting medicine:", error);
        alert("Error deleting medicine.");
    }
}

// Function to handle updating a medicine
async function updateMedicine() {
    const id = prompt("Enter Medicine ID to update:");
    const name = prompt("Enter new Medicine Name:");
    const price = prompt("Enter new Medicine Price:");

    const data = { action: "update", id, name, price };

    try {
        let response = await fetch('http://127.0.0.1:8080', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(data)
        });

        if (response.ok) {
            alert("Medicine updated successfully!");
        } else {
            console.error("Failed to update medicine:", response.statusText);
            alert("Failed to update medicine.");
        }
    } catch (error) {
        console.error("Error updating medicine:", error);
        alert("Error updating medicine.");
    }
}

// Function to list all medicines
async function listMedicines() {
    try {
        let response = await fetch('http://127.0.0.1:8080');

        if (response.ok) {
            let data = await response.json();
            const medicineList = document.getElementById("medicines");
            medicineList.innerHTML = "";
            data.medicines.forEach(medicine => {
                const listItem = document.createElement("li");
                listItem.textContent = `ID: ${medicine.id}, Name: ${medicine.name}, Price: ${medicine.price}`;
                medicineList.appendChild(listItem);
            });
        } else {
            console.error("Failed to list medicines:", response.statusText);
            alert("Failed to list medicines.");
        }
    } catch (error) {
        console.error("Error listing medicines:", error);
        alert("Error listing medicines.");
    }
}
