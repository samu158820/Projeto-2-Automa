# Projeto-2-Automa
Projeto de automação - Controle de temperatura proporcional com Sloeber e Arduino

# Descrição do projeto:
  Este projeto tem como objetivo o controle de temperatura proporcional.
  
A leitura realizada pelo sensor de temperatura é comparada com o setPoint configurado pelo usuário.

O setPoint inicial é definido pelo programador.

O programa pode ser visto como uma máquina de estados.

Um led irá ascender quando o sistema estiver conectado à energia, e piscará a cada 500ms (2Hz) quando o driver estiver ligado.
  
  Os possíveis estados são:
		menu 	- mostra valores e status de driver;
		
		spconf 	- configuração de valor de setPoint, a temperatura que se deseja;
		
		KPconf	- configuração do valor de ganho, sendo dividido em quartis (25% à 100%);
		
		dvrconf	- habilitar / desabilitar driver, ligando ou desligando o PWM do aquecedor e do ventilador;
		
		manHeat	- aquecedor no modo manual, funciona independentemente do ganho, alterado agora em percentis
		
		manFan	- ventilador no modo manual, funciona independentemente do ganho, alterado agora em percentis
		
	Quando não se está em modo manual, a temperatura atual é ajustada conforme o setPoint, ligando ou desligando os componentes (aquecedor e ventilador) com PWM.
