$$
\begin{align*}
[\text{program}] &\to [\text{statement}]~[\text{program}] \mid \epsilon
\\\\
[\text{statement}] &\to [\text{type}] ~ \text{identifier} ~ [\text{statement\_tail}] \mid  \text{print} ~ \text{(}[\text{print\_expr}] \text{)} ~ \text{;}
\\
[\text{statement\_tail}] &\to \text{;} \mid = ~ [\text{expression}]~\text{;} \mid \text{(} [\text{parameter\_list}] \text{)} [\text{block}]
\\\\
[\text{print\_expr}] &\to [\text{print\_term}]~[\text{print\_expr}']
\\
[\text{print\_expr}'] &\to + [\text{print\_term}]~[\text{print\_expr}'] \mid \epsilon
\\
[\text{print\_term}] &\to \text{string} \mid \text{identifier}
\\\\
[\text{expression}] &\to [\text{term}]~[\text{expression'}]
\\
[\text{expression'}] &\to + [\text{term}]~[\text{expression'}] \mid - [\text{term}]~[\text{expression'}] \mid \epsilon
\\\\
[\text{term}] &\to [\text{factor}]~[\text{term'}]
\\
[\text{term'}] &\to * [\text{factor}]~[\text{term'}] \mid / [\text{factor}]~[\text{term'}] \mid \epsilon
\\\\
[\text{factor}] &\to \text{number} \mid ([\text{expression}])
\\\\
[\text{type}] &\to \text{int} \mid \text{identifier} \mid \text{uint}
\end{align*}
$$