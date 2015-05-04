seq1 <- seq(1000:1999)
mat1 <- matrix(seq1,50)

seq2 <- seq(3000:3999)
mat2 <- matrix(seq2,50)

mat3 <- mat1 %*% t(mat2)

proc.time()

