import { useState } from 'react'
import { loginUser, registerUser } from '../APIManager'
import { useNavigate } from 'react-router-dom';
import {
  Box,
  Container,
  Typography,
  TextField,
  Button,
  Link,
  Grid
} from '@mui/material'
import { useGlobal } from '../SessionManager';

function Register(){
    const [username, setUsername] = useState('')
    const [password, setPassword] = useState('')
    const [confirmedPassword, setConfirmedPassword] = useState('')

    const navigator = useNavigate();

    function handleLogin(){
        if (password != confirmedPassword) {
            alert("Password Does not Match!")
            return;
        }

        registerUser(username, password).then(data=>{
            if (data) navigator('/')
        });
    }

    return (
        <Box sx={{
        minHeight: '92vh',
        background: `linear-gradient(135deg, rgba(102, 126, 234, 0.1), rgba(118, 75, 162, 0.1)), url('/background.jpg')`,
        backgroundSize: 'cover',
        backgroundPosition: 'center',
        backgroundAttachment: 'fixed',
        py: 0,
        px: 2,
      }}>
        <Container maxWidth="xs" sx={{ py: 6 }}>
        
            <Typography variant="h3" fontWeight={600} sx={{ mb: '10%' }}>
                Register: 
            </Typography>

            <TextField
                label="Username"
                value={username}
                onChange={(event) => setUsername(event.target.value)}
                fullWidth
                sx={{mb: '5%'}}
            />

            <TextField
                label="Password"
                type="password"
                autoComplete="current-password"
                value={password}
                onChange={(event) => setPassword(event.target.value)}
                fullWidth
                sx={{mb: '5%'}}
            />

            <TextField
                label="Confirm Password"
                type="password"
                autoComplete="current-password"
                value={confirmedPassword}
                onChange={(event) => setConfirmedPassword(event.target.value)}
                fullWidth
                sx={{mb: '5%'}}
            />

            <Grid container justifyContent="space-between">
                <Grid item>
                    <Link href="/login">
                        Back to Log In
                    </Link>
                </Grid>
                <Grid item>
                    <Button variant="contained" onClick={handleLogin}>
                        Register
                    </Button>
                </Grid>
            </Grid>
        </Container></Box>
    )
}

export default Register