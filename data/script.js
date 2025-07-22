let itens = []
let currentProductIdToDelete = null; // Variável para armazenar o ID do produto a ser excluído
let currentProductNameToDelete = null;

async function getItems() {
  response = await fetch('http://192.168.107.122/api/inventory')
  const json = await response.json();
  itens = json.itens
}

function goToCreatePage(){
  window.location.href = "/form";
}
function goToEditPage(id){
    window.location.href = `/formEdit/${id}`;
}

function showDeletePopover(productId, productName) {  
    currentProductIdToDelete = productId;
    currentProductNameToDelete = productName;
    document.getElementById('productToDeleteName').textContent = productName; // Exibe o nome do produto no popover
    document.getElementById('deleteConfirmPopover').classList.add('visible');
}

function hideDeletePopover() {
    document.getElementById('deleteConfirmPopover').classList.remove('visible');
    currentProductIdToDelete = null; 
    currentProductNameToDelete = null; 
}
async function performDelete() {
    if (!currentProductIdToDelete) return; // Garante que há um ID para excluir

    console.log("Confirmando exclusão do produto ID:", currentProductIdToDelete);
    try {
        const response = await fetch(`/api/deleteProduct/${currentProductIdToDelete}`, {
            method: 'DELETE', // Método DELETE para exclusão
        });

        if (response.ok) {
            alert('Produto excluído com sucesso!');
            hideDeletePopover(); // Esconde o popover
            await getItems(); // Recarrega os dados do inventário
            populateProductTable(itens); // Atualiza a tabela
        } else {
            const errorText = await response.text();
            alert('Erro ao excluir produto: ' + errorText);
            hideDeletePopover();
        }
    } catch (error) {
        console.error('Erro na requisição de exclusão:', error);
        alert('Erro de conexão ao tentar excluir o produto.');
        hideDeletePopover();
    }
}
function handleAction(event) {
  // Check if the clicked element or its parent is a button
  let targetButton = event.target.closest('button');

  if (targetButton) {
    // Get the ID of the product from the row (you'll need to add a data-id attribute to your row or button)
    // For this example, let's assume the button itself can hold the product ID, or you get it from the parent row.
    // Let's add a data-product-id attribute to the buttons for clarity
    const productId = targetButton.dataset.productId;
    const productRow = targetButton.closest('tr');
    const productName = productRow ? productRow.querySelector('.column-product-name').textContent : 'este item';
    
    if (targetButton.id === "editStock") {
      goToEditPage(productId)
      console.log("Botão de Edição clicado para o produto ID:", productId);
      // Implement your edit logic here
    } else if (targetButton.id === "deleteStock") {
      showDeletePopover(productId, productName);
      console.log("Botão de Excluir clicado para o produto ID:", productId);
      // Implement your delete logic here
      // For example: if (confirm('Tem certeza que deseja excluir este produto?')) { deleteProduct(productId); }
    }
  }
}

async function populateProductTable(data) {
  console.log(data,"dados");
  
      const tableBody = document.querySelector('table tbody');
      tableBody.innerHTML = ''; // Limpa qualquer conteúdo existente

      data.forEach(product => {
          const row = document.createElement('tr');
          row.innerHTML = `
              <td class="column-product-name">${product.name}</td>
              <td class="column-quantity quantity">${product.amount}</td>
              <td class="column-price price">$${product.price}</td>
              <td class="column-price price">${product.category}</td>
              <td class="column-price price">$${product.cost}</td>
              <td class="column-price price">${product.location}</td>
              <td class="column-actions actions">
               <button id="editStock" data-product-id="${product.id}" title="Editar Produto" style="background: none; border: none; cursor: pointer; color: #974e73; font-size: 1.2em; margin-right: 10px;">
                <i class="fa-solid fa-pencil"></i>
              </button>
              <button id="deleteStock" data-product-id="${product.id}" title="Excluir Produto" style="background: none; border: none; cursor: pointer; color: #974e73; font-size: 1.2em;">
                <i class="fa-solid fa-trash-can"></i>
              </button>
              </td>
          `;
          tableBody.appendChild(row);
      });
  }
document.addEventListener('DOMContentLoaded', async () => {
  await getItems(); // Fetch data
  populateProductTable(itens); // Populate table with fetched data

  // Event delegation for Edit and Delete buttons
  const tableBody = document.querySelector('table tbody');
  if (tableBody) { // Ensure tableBody exists
    tableBody.addEventListener('click', handleAction);
  }

  // Event listener for "Add Product" button (assuming it has id="addProductButton" in your HTML)
  const addProductButton = document.getElementById("addProductButton");
  if (addProductButton) {
    addProductButton.addEventListener("click", goToCreatePage);
  }
    document.getElementById('cancelDeleteButton').addEventListener('click', hideDeletePopover);
  document.getElementById('confirmDeleteButton').addEventListener('click', performDelete);
});
