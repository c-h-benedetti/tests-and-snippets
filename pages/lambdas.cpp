
/*!

\page lambdaexprpage ⊹ Notes λ-expressions

## Généralités:

### La liste de captures, qui permet de capturer des variables externes pour les utiliser dans la lambda expression. On peut spécifier les captures de différentes manières :

- `[&]` pour capturer toutes les variables par référence ;
- `[=]` pour capturer toutes les variables par copie ;
- `[var1, var2]` pour capturer des variables spécifiques par copie ;
- `[&var1, &var2]` pour capturer des variables spécifiques par référence;
- `[v = expr]` pour faire une "init capture", c'est-à-dire créer une nouvelle variable valable dans le corps de l'expression.

### Les spécificateurs de fonction, qui permettent de définir des propriétés supplémentaires pour la lambda expression, tels que la constance ou la volatilité. On peut spécifier les spécificateurs de fonction de différentes manières :

- `() -> int` pour spécifier le type de retour de la lambda expression ;
- `() const` pour rendre la lambda expression constante ;
- `() noexcept` pour spécifier que la lambda expression ne lance pas d'exception ;
- `() -> int const &` pour spécifier le type de retour de la lambda expression comme une référence constante.
 
### Les paramètres de la lambda expression, qui sont spécifiés après les spécificateurs de fonction et qui décrivent les arguments que la lambda expression prend. On peut spécifier les paramètres de différentes manières :
 
- `()` pour spécifier une lambda expression qui ne prend aucun argument ;
- `(int x, double y)` pour spécifier une lambda expression qui prend deux arguments, un int nommé x et un double nommé y.

__Note:__ Les crochets peuvent être laissés vides si aucun comportement spécial n'est requis.

---

## 📌 Init capture

Dans les crochets, il est possible de créer une nouvelle variable utilisable dans le corps de l'expression.
Modifier les variables externes après la déclaration ne sert à rien, la variable est évaluée à la définition.

\snippet 003-lambda-expressions/main.cpp init_capture

## 📌 Avec SigC

Grace aux λ-expressions, on peut remplacer la première ligne par la seconde, qui est nettement plus lisible. Il est même possible de donner directement le corps dans l'appel de la fonction dans ce cas.

\code{.cpp}
GLArea glArea;
glArea.signal_realize().connect(sigc::mem_fun(glArea, &GLArea::on_realize_signal));
glArea.signal_realize().connect([&glArea](){ glArea.on_realize_signal(); });
\endcode

*/