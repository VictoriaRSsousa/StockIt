function goToHomePage(){
    window.location.href = '/'
}

async function submit(){
    event.preventDefault();
    console.log("enviado")

}
  const productForm = document.getElementById('productForm');

    // Adicione um ouvinte de evento para a submissão do formulário
    // É MELHOR TRATAR O EVENTO DE 'submit' DO FORMULÁRIO, NÃO DO BOTÃO 'onclick'
    productForm.addEventListener('submit', async (event) => {
        // 1. PREVINA O COMPORTAMENTO PADRÃO DO FORMULÁRIO (IMPEDE A RECARGA DA PÁGINA)
        event.preventDefault();

        console.log("enviado"); // Agora este console.log será visível!

        // Seus passos para coletar os dados do formulário
        const formData = new FormData(productForm);
        const productData = {};
        for (let [key, value] of formData.entries()) {
            // Converta campos numéricos para números
            if (['amount', 'critic', 'price', 'cost'].includes(key)) {
                productData[key] = parseFloat(value);
            } else {
                productData[key] = value;
            }
        }

        // Validação básica
        if (!productData.id || !productData.name || !productData.amount || !productData.critic || !productData.price) {
            alert('Por favor, preencha todos os campos obrigatórios (ID, Descrição, Quantidade, Estoque Crítico, Preço de Venda).');
            return;
        }

        console.log('Dados do produto a serem enviados:', productData);

        // Enviar os dados JSON para o ESP32
        try {
            const response = await fetch('/api/addProduct', { // Certifique-se de que este é o endpoint correto no seu ESP32
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(productData)
            });

            if (response.ok) {
                alert('Produto criado com sucesso!');
                productForm.reset(); // Limpa o formulário
                // Opcionalmente redirecione ou atualize uma lista após o sucesso
                // window.location.href = "/products";
            } else {
                const errorText = await response.text();
                alert('Erro ao criar produto: ' + errorText);
            }
        } catch (error) {
            console.error('Erro na requisição:', error);
            alert('Erro de conexão ao tentar criar o produto.');
        }
    });

   const cancelButton = document.getElementById('cancelButton');
  if (cancelButton) {
    cancelButton.addEventListener("click", goToHomePage);
  }