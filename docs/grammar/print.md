$$
\begin{align*}
[\text{print\_expr}] &\to [\text{print\_term}]~[\text{print\_expr}']
\\
[\text{print\_expr}'] &\to + [\text{print\_term}]~[\text{print\_expr}'] \mid \epsilon
\\
[\text{print\_term}] &\to \text{string} \mid \text{identifier}
\end{align*}
$$