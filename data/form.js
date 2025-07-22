// Funções auxiliares, podem ficar no topo do seu arquivo JS
function goToHomePage() {
    window.location.href = '/';
}

// Função genérica para coletar dados do formulário e converter tipos
function getFormDataAsObject(formElement) {
    const formData = new FormData(formElement);
    const data = {};
    for (let [key, value] of formData.entries()) {
        if (['amount', 'critic', 'price', 'cost'].includes(key)) {
            const numValue = parseFloat(value);
            // Se não for um número válido, define como 0 ou null, ou mantém como string vazia
            data[key] = isNaN(numValue) ? null : numValue;
        } else {
            data[key] = value;
        }
    }
    return data;
}
// --- Lógica Principal ---
document.addEventListener('DOMContentLoaded', () => {
    const productForm = document.getElementById('productForm'); // Formulário de Criação
    const productEditForm = document.getElementById('productEditForm'); // Formulário de Edição
    const cancelButton = document.getElementById('cancelButton'); // Botão de cancelar para o form de criação
    const cancelEditButton = document.getElementById('cancelEditButton'); // Botão de cancelar para o form de edição

    // Lógica para o formulário de CRIAÇÃO
    if (productForm) {
        productForm.addEventListener('submit', async (event) => {
            event.preventDefault();
            console.log("Formulário de criação enviado.");

            const productData = getFormDataAsObject(productForm);

            // Validação básica
            if (!productData.id || !productData.name || !productData.amount || !productData.critic || !productData.price) {
                alert('Por favor, preencha todos os campos obrigatórios (ID, Descrição, Quantidade, Estoque Crítico, Preço de Venda).');
                return;
            }

            console.log('Dados do produto a serem enviados (Criação):', productData);

            try {
                const response = await fetch('/api/addProduct', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'text/plain' // MUITO IMPORTANTE: Ajustado para 'text/plain'
                    },
                    body: JSON.stringify(productData) // Envia o JSON como string
                });

                if (response.ok) {
                    alert('Produto criado com sucesso!');
                    productForm.reset(); // Limpa o formulário de criação
                } else {
                    const errorText = await response.text();
                    alert('Erro ao criar produto: ' + errorText);
                }
            } catch (error) {
                console.error('Erro na requisição de criação:', error);
                alert('Erro de conexão ao tentar criar o produto.');
            }
        });
    }

    // Lógica para o formulário de EDIÇÃO
    if (productEditForm) {
        // Obtenha o ID do produto da URL (para preencher o formulário)
        const path = window.location.pathname;
        let currentProductId = null;
        if (path.startsWith('/formEdit/')) {
            currentProductId = path.substring(path.lastIndexOf('/') + 1);
            console.log('Modo de edição: ID do produto:', currentProductId);

            // **PREENCHER O FORMULÁRIO COM DADOS EXISTENTES (Lógica da resposta anterior)**
            if (currentProductId) {
                fetch(`/api/product/${currentProductId}`) // Endpoint para buscar dados por ID
                    .then(response => {
                        if (!response.ok) {
                            throw new Error(`Erro HTTP ao carregar dados! Status: ${response.status}`);
                        }
                        return response.json();
                    })
                    .then(data => {
                        console.log('Dados do produto carregados para edição:', data);
                        // Preencha os campos do formulário de edição com 'data'
                        // Exemplo:
                        document.getElementById('productIdEdit').value = data.id || ''; // Certifique-se de ter IDs únicos no formEdit.html
                        document.getElementById('productNameEdit').value = data.name || '';
                        document.getElementById('productAmountEdit').value = data.amount || '';
                        document.getElementById('productCriticEdit').value = data.critic || '';
                        document.getElementById('productPriceEdit').value = data.price || '';
                        document.getElementById('productCostEdit').value = data.cost || '';
                        document.getElementById('productCategoryEdit').value = data.category || '';
                        document.getElementById('productLocationEdit').value = data.location || '';

                        // O campo ID geralmente é read-only na edição
                        const idField = document.getElementById('productIdEdit');
                        if (idField) idField.readOnly = true;

                    })
                    .catch(error => {
                        console.error('Erro ao carregar dados para edição:', error);
                        alert('Erro ao carregar dados do produto para edição. Verifique o ID.');
                    });
            }
        }

        productEditForm.addEventListener('submit', async (event) => {
            event.preventDefault();
            console.log("Formulário de edição enviado.");
            console.log(productEditForm);
            
            const productData = getFormDataAsObject(productEditForm);
                    console.log(productData);
                    
            // Validação básica (garantir que o ID está presente para a edição)
            if (!productData.id || !productData.name || !productData.amount || !productData.critic || !productData.price) {
                alert('Por favor, preencha todos os campos obrigatórios (ID, Descrição, Quantidade, Estoque Crítico, Preço de Venda).');
                return;
            }

            console.log('Dados do produto a serem enviados (Edição):', productData);

            try {
                // MÉTODO E ROTA DEVEM COMBINAR COM SEU C++ NO server_routes.cpp
                // Se no C++ é server.on("/api/editProduct", HTTP_POST, ...) então aqui é POST.
                const response = await fetch('/api/editProduct', {
                    method: 'POST', // Ajuste para 'POST' ou 'PUT' conforme seu backend C++
                    headers: {
                        'Content-Type': 'text/plain' // MUITO IMPORTANTE: Ajustado para 'text/plain'
                    },
                    body: JSON.stringify(productData)
                });
                console.log(response);
                

                if (response.ok) {
                    alert('Produto atualizado com sucesso!');
                    window.location.href = "/"; // Redireciona para a home page após a edição
                } else {
                    const errorText = await response.text();
                    alert('Erro ao atualizar produto: ' + errorText);
                }
            } catch (error) {
                console.error('Erro na requisição de edição:', error);
                alert('Erro de conexão ao tentar atualizar o produto.');
            }
        });
    }

    // Configurar botões de cancelar
    if (cancelButton) {
        cancelButton.addEventListener("click", goToHomePage);
    }
    if (cancelEditButton) {
        cancelEditButton.addEventListener("click", goToHomePage);
    }
});